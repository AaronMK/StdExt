#include <StdExt/Concurrent/SysTask.h>

#include <StdExt/Concurrent/Task.h>

namespace StdExt::Concurrent
{
#if defined(STD_EXT_COROUTINE_TASKS)
	std::suspend_always SysTask::promise_type::initial_suspend()
	{
		return{};
	}

	std::suspend_always SysTask::promise_type::final_suspend() noexcept
	{
		return{};
	}

	void SysTask::promise_type::return_void()
	{
		return;
	}

	void SysTask::promise_type::unhandled_exception()
	{
		mException = std::current_exception();
	}

	SysTask SysTask::promise_type::get_return_object()
	{
		return SysTask( handle_t::from_promise(*this) );
	}

	SysTask::SysTask(handle_t&& handle)
	{
		mHandle = std::move(handle);
	}

	SysTask::SysTask(SysTask&& other)
		: mHandle( std::move(other.mHandle) )
	{
		other.mHandle = nullptr;
	}

	SysTask::SysTask()
		: mHandle(nullptr)
	{
	}

	SysTask::~SysTask()
	{
		if ( mHandle )
			mHandle.destroy();
	}

	SysTask& SysTask::operator=(SysTask&& other)
	{
		assert(nullptr == mHandle);
		
		mHandle = other.mHandle;
		other.mHandle = nullptr;

		return *this;
	}

	SysTask::operator bool() const
	{
		return (nullptr != mHandle);
	}

	const SysTask::promise_type* SysTask::getPromise() const
	{
		return access_as<const promise_type*>( std::addressof(mHandle.promise()) );
	}

	SysTask::promise_type* SysTask::getPromise()
	{
		return access_as<promise_type*>( std::addressof(mHandle.promise()) );
	}

	const TaskBase* SysTask::getTask() const
	{
		return mHandle.promise().mParent;
	}

	TaskBase* SysTask::getTask()
	{
		return mHandle.promise().mParent;
	}

	bool SysTask::isDone() const
	{
		return ( nullptr == mHandle || mHandle.done() );
	}

	void SysTask::resume()
	{
		mHandle.resume();
	}

	SysTask SysTask::makeCoroutine(TaskBase* parent)
	{
		parent->run_task();
		co_return;
	}
#elif defined(STD_EXT_APPLE)
	SysTask::SysTask(dispatch_block_t db)
		: mDispatchBlock(db)
	{
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
#endif
}