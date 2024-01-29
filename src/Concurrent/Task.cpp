#include <StdExt/Concurrent/Task.h>

#if defined(STD_EXT_APPLE)
#	include <Block.h>
#endif

#include <StdExt/Utility.h>
#include <StdExt/Exceptions.h>

namespace StdExt::Concurrent
{
	static void throwDormant()
	{
		throw invalid_operation("Attempting to wait on a dormant task.");
	}

	static void throwTimeout()
	{
		throw time_out("Task timed out while waiting for completion.");
	}

	TaskBase::TaskBase()
	{
		mState = TaskState::Dormant;
	}

	TaskBase::~TaskBase()
	{
	}

#if defined(STD_EXT_APPLE)
	SysTask::SysTask(TaskBase* parent, dispatch_block_t db)
		: mParent(parent), mDispatchBlock(db)
	{
		mDispatchBlock = nullptr;
	}

	SysTask::~SysTask()
	{
		Block_release(mDispatchBlock);
	}

	void TaskBase::wait(const Chrono::Milliseconds& ms_timeout)
	{
		TaskState task_state = mState;

		if ( !mSysTask.has_value() || TaskState::Dormant == task_state )
			throwDormant();

		if ( TaskState::Finished == task_state )
			return;

		auto sys_timeout = ( ms_timeout.count() > 0.0 ) ?
			dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(Chrono::Nanoseconds(ms_timeout).count()) ) :
			dispatch_time(DISPATCH_TIME_FOREVER, 0);

		if ( 0 != dispatch_block_wait(mSysTask->mDispatchBlock, sys_timeout) )
			throwTimeout();
	}

#elif defined(STD_EXT_WIN32)
	SysTask::SysTask(TaskBase* parent, concurrency::Scheduler& sys_scheduler)
		: concurrency::agent(sys_scheduler), mParent(parent)
	{
	}

	SysTask::~SysTask()
	{
	}

	void SysTask::run()
	{
		mParent->run_task();
		done();
	}

	TaskState TaskBase::state() const
	{
		using namespace concurrency;

		if ( mSysTask )
		{
			agent* non_const_agent = access_as<agent*>(std::addressof(*mSysTask));
			agent_status status = non_const_agent->status();

			switch(status)
			{
			case agent_status::agent_canceled:
			case agent_status::agent_done:
				return TaskState::Finished;
			case agent_status::agent_created:
			case agent_status::agent_runnable:
				return TaskState::InQueue;
			case agent_status::agent_started:
				return TaskState::Running;
			}
		}

		return mState;
	}

	void TaskBase::wait(const Chrono::Milliseconds& ms_timout)
	{
		using namespace concurrency;
		
		if ( TaskState::Dormant == mState )
			throwDormant();

		if ( mSysTask )
		{
			try
			{
				auto wait_arg = (ms_timout.count() > 0) ?
					static_cast<unsigned int>(ms_timout.count()) :
					COOPERATIVE_TIMEOUT_INFINITE;
			
				agent_status status = agent::wait(
					std::addressof(*mSysTask), wait_arg
				);

				switch(status)
				{
				case agent_status::agent_done:
					return;
				case agent_status::agent_started:
				case agent_status::agent_runnable:
					throwTimeout();
					break;
				case agent_status::agent_canceled:
				case agent_status::agent_created:
					throwDormant();
					break;
				default:
					throw std::runtime_error("Unknown TaskBase::wait() error.");
				}
			}
			catch ( const operation_timed_out& )
			{
				throwTimeout();
			}
		}
	}

	void TaskBase::reset()
	{
		TaskState task_state = state();
		
		if ( TaskState::Dormant != task_state && TaskState::Finished != task_state )
			throw invalid_operation("Cannot reset a task while it is running.");

		mException = nullptr;
		mState = TaskState::Dormant;
		mSysTask.reset();
	}
#elif defined(STD_EXT_GCC)
	TaskBase::TaskBase()
	{
		mTaskState = TaskState::Dormant;
	}

	TaskBase::~TaskBase()
	{
	}

	void TaskBase::internalWait(Chrono::Milliseconds timeout)
	{
		using namespace std;
		
		TaskState task_state = mTaskState;

		if ( TaskState::Finished == task_state )
			return;

		if ( TaskState::Dormant == task_state )
			throwDormant();

		if (timeout.count() > 0)
		{
			switch( mFuture.wait_for(timeout) )
			{
			case future_status::deferred:
				throwDormant();
			case future_status::timeout:
				throwTimeout();
			}

			throw std::runtime_error("TaskBase::internalWait() - Unknown error. (GCC)");
		}
		else
		{
			mFuture.wait();
		}

		if ( mException )
			std::rethrow_exception(mException);
	}
#endif
}
