#include <StdExt/Tasking/SysTask.h>

#include "internal/ThreadPool.h"

#include <atomic>
#include <queue>
#include <set>
#include <thread>

namespace StdExt::Tasking
{
#if defined(STD_EXT_COROUTINE_TASKS)

#pragma region promise_type
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
		handle_t handle = handle_t::from_promise(*this);

		ThreadPool::TaskSync.trigger(
			[&]() -> size_t
			{
				ThreadPool::ReadyTaskQueue.push( mParent );
				size_t ready_count = ThreadPool::ReadyTaskQueue.size();

				if ( ready_count <= ThreadPool::MaxConcurrency && ThreadPool::ActiveExecuterCount < ready_count )
				{
					++ThreadPool::ActiveExecuterCount;

					std::thread exec_thread(&ThreadPool::executerMain);
					exec_thread.detach();
				}

				return ready_count;
			}
		);

		return SysTask(handle);
	}

	SysTask::handle_t SysTask::promise_type::getHandle()
	{
		return std::coroutine_handle<promise_type>::from_promise(*this);
	}

#pragma endregion

	SysTask::SysTask(handle_t handle)
	{
		mHandle = std::move(handle);
	}

	SysTask::SysTask(SysTask&& other) noexcept
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

	const Task* SysTask::getTask() const
	{
		return mHandle.promise().mParent;
	}

	Task* SysTask::getTask()
	{
		return mHandle.promise().mParent;
	}

	const  SysTask::handle_t SysTask::getHandle() const
	{
		return mHandle;
	}

	SysTask::handle_t  SysTask::getHandle()
	{
		return mHandle;
	}

	bool SysTask::isDone() const
	{
		return ( nullptr == mHandle || mHandle.done() );
	}

	void SysTask::resume()
	{
		mHandle.resume();
	}

	SysTask::SysSyncTasking::SysSyncTasking()
	{
	}

	SysTask::SysSyncTasking::~SysSyncTasking()
	{
	}

	void SysTask::SysSyncTasking::suspend()
	{
	}

	void SysTask::SysSyncTasking::wake()
	{
	};

#elif defined(STD_EXT_APPLE)
	SysTask::SysTask(dispatch_block_t db)
		: mDispatchBlock(db)
	{
	}

	SysTask::~SysTask()
	{
		Block_release(mDispatchBlock);
	}

	void Task::wait(const Chrono::Milliseconds& ms_timeout)
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
	SysTask::SysTask(Task* parent, concurrency::Scheduler& sys_scheduler)
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

	TaskState Task::state() const
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