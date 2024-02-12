#include <StdExt/Tasking/SysTask.h>

#include <atomic>
#include <queue>
#include <set>
#include <thread>

namespace StdExt::Tasking
{
#if defined(STD_EXT_COROUTINE_TASKS)

#pragma region Threadpool
	static SyncPoint            PoolSyncPoint;

	static const uint32_t       MaxConcurrency = std::thread::hardware_concurrency() + 2;
	static std::atomic<int>     ActiveExecutorCount = 0;
	static std::set<SysTask::handle_t>   BlockedTaskQueue;
	static std::queue<SysTask::handle_t> ReadyTaskQueue;

	static thread_local std::atomic_flag  executor_sync_flag;
	static thread_local SysTask::handle_t active_task_handle;

	static void runExecutor()
	{
		using WaitState = SyncPoint::WaitState;
		
		class GetTaskInterface : public SyncInterface
		{
		public:
			SysTask::handle_t task_handle;

			GetTaskInterface()
			{
				task_handle = nullptr;
				executor_sync_flag.test_and_set();
			}

			bool testPredicate() override
			{
				return (ReadyTaskQueue.size() > 0);
			}

			void atomicAction() override
			{
				task_handle = ReadyTaskQueue.front();
				ReadyTaskQueue.pop();
			}

			void markForSuspend() override
			{
				executor_sync_flag.clear();
			}

			void atomicFailHandler(WaitState result) override
			{
				// Since this will cause the executor to let itself complete the thread,
				// decrement in SyncPoint context to prevent race conditions.
				--ActiveExecutorCount;
			}

			void wake() override
			{
				executor_sync_flag.test_and_set();
				executor_sync_flag.notify_one();
			}

			void wait()
			{
				executor_sync_flag.wait(false);
			}
		};

		auto updateActiveSysTask = [&]() -> bool
		{
			GetTaskInterface sync_interface;

			PoolSyncPoint.wait(&sync_interface);
			sync_interface.wait();

			if ( WaitState::Complete != sync_interface.waitState() )
			{
				--ActiveExecutorCount;
				return false;
			}

			active_task_handle = sync_interface.task_handle;
			return true;
		};

		while ( updateActiveSysTask() )
		{
			active_task_handle.resume();

			if ( active_task_handle.done() )
			{
				// Do task completion alerts.
			}
			else
			{
				PoolSyncPoint.trigger(
					[&]()
					{
						BlockedTaskQueue.insert(active_task_handle);
						active_task_handle = nullptr;

						return false;
					}
				);
				
			}
		}

		active_task_handle = nullptr;
		executor_sync_flag.clear();
	}
#pragma endregion

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

		PoolSyncPoint.trigger(
			[&]() -> size_t
			{
				ReadyTaskQueue.push(handle);
				size_t ready_count = ReadyTaskQueue.size();

				if ( ready_count <= MaxConcurrency && ActiveExecutorCount < ready_count )
				{
					++ActiveExecutorCount;

					std::thread exec_thread(&runExecutor);
					exec_thread.detach();
				}

				return ready_count;
			}
		);

		return SysTask(handle);
	}
#pragma endregion

	SysTask::SysTask(handle_t handle)
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

	SysTask::SysSyncTasking::SysSyncTasking()
	{
	}

	SysTask::SysSyncTasking::~SysSyncTasking()
	{
	}

	void SysTask::SysSyncTasking::markForSuspend()
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