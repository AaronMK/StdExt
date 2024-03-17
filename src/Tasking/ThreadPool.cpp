#include "internal/ThreadPool.h"

#include "StdExt/Tasking/Task.h"

namespace StdExt::Tasking
{
	SyncPoint          ThreadPool::TaskSync;

	static std::queue<Task*> ReadyTaskQueue;

	static const size_t MaxConcurrency        = std::thread::hardware_concurrency();
	static size_t       StartingExecuterCount = 0;
	static size_t       ActiveExecuterCount   = 0;

	static thread_local std::atomic_flag  executor_sync_flag;
	static thread_local Task*             current_task = nullptr;

	void ThreadPool::addTask(Task* task)
	{
		TaskSync.trigger(
			[&]()
			{
				task->mState = TaskState::InQueue;
				ReadyTaskQueue.push(task);

				if ( ActiveExecuterCount < ReadyTaskQueue.size() &&
				     ActiveExecuterCount < MaxConcurrency )
				{
					++ActiveExecuterCount;
					++StartingExecuterCount;
					std::thread(&executerMain).detach();
				}
			}
		);
	}

	void ThreadPool::executerMain()
	{
		class InitTaskWait : public AtomicTaskSync
		{
		public:
			InitTaskWait(std::atomic_flag* flag)
				: AtomicTaskSync(flag)
			{}

		protected:
			bool testPredicate() override
			{
				return (ReadyTaskQueue.size() > 0);
			}

			void atomicAction(WaitState state) override
			{
				switch ( state )
				{
				case WaitState::PredicateSatisfied:
					current_task = ReadyTaskQueue.front();
					ReadyTaskQueue.pop();

					current_task->mState = TaskState::Running;
					break;
				}

				if ( state != WaitState::Waiting )
					--StartingExecuterCount;
			}
		};

		{
			std::atomic_flag sync_flag;
			InitTaskWait init_waiter(&sync_flag);
			TaskSync.wait(&init_waiter);
			init_waiter.clientWait();
		}

		while ( current_task )
		{
			current_task->run_task();

			TaskSync.protectedAction(
				[&]()
				{
					current_task->mState = TaskState::Finished;
					current_task->mFinished.setValue(true);
					current_task = nullptr;
					
					if ( ReadyTaskQueue.size() > 0 )
					{
						current_task = ReadyTaskQueue.front();
						ReadyTaskQueue.pop();
					}
					else
					{
						--ActiveExecuterCount;
					}
				}
			);
		}
	}

	bool ThreadPool::isActive()
	{
		return (nullptr != current_task);
	}

	ThreadPoolSync::ThreadPoolSync()
		: AtomicTaskSync(&executor_sync_flag)
	{
		mInThreadPool = false;
	}

	ThreadPoolSync::~ThreadPoolSync()
	{
	}
	
	void ThreadPoolSync::suspend()
	{
		mInThreadPool = ThreadPool::isActive();
		
		if ( mInThreadPool )
		{
			current_task->mState = TaskState::Blocked;
			
			ThreadPool::TaskSync.protectedAction(
				[]()
				{
					assert(ActiveExecuterCount > 0);
					--ActiveExecuterCount;

					if ( ReadyTaskQueue.size() > StartingExecuterCount )
					{
						++StartingExecuterCount;
						std::thread(&ThreadPool::executerMain).detach();
					}
				}
			);
		}
		
		AtomicTaskSync::suspend();
	}
	
	void ThreadPoolSync::wake()
	{
		if ( mInThreadPool )
		{
			ThreadPool::TaskSync.protectedAction(
				[]()
				{
					++ActiveExecuterCount;
				}
			);
		}

		mInThreadPool = false;
		AtomicTaskSync::wake();
	};

	ThreadPoolSyncAwaiter::ThreadPoolSyncAwaiter(SyncPoint* sync_point, ThreadPoolSync* sync_interface)
		: SyncAwaiter(sync_point, sync_interface)
	{
	}

	bool ThreadPoolSyncAwaiter::await_suspend(std::coroutine_handle<> handle)
	{
		return true;
	}
}