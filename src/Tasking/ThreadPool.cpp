#include "internal/ThreadPool.h"

#include "StdExt/Tasking/Task.h"

namespace StdExt::Tasking
{
	thread_local Task* ThreadPool::CurrentTask = nullptr;
	SyncPoint          ThreadPool::TaskSync;

	std::queue<SysTask::handle_t> ThreadPool::ReadyTaskQueue;
	std::set<SysTask::handle_t>   ThreadPool::BlockedTaskQueue;

	const uint32_t   ThreadPool::MaxConcurrency = std::thread::hardware_concurrency();
	std::atomic<int> ThreadPool::ActiveExecutorCount = 0;

	static thread_local std::atomic_flag  executor_sync_flag;
	static thread_local SysTask::handle_t active_task_handle;

	void ThreadPool::addExecuter()
	{
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

			void atomicAction(WaitState result) override
			{
				switch ( result )
				{
				case WaitState::PredicateSatisfied:
					task_handle = ReadyTaskQueue.front();
					ReadyTaskQueue.pop();
					break;
				case WaitState::Timeout:
				case WaitState::Destroyed:
				case WaitState::Canceled:
					// Since this will cause the executor to let itself complete the thread,
					// decrement in SyncPoint context to prevent race conditions.
					--ActiveExecutorCount;
					break;
				}
			}

			void markForSuspend() override
			{
				executor_sync_flag.clear();
			}

			void wake() override
			{
				executor_sync_flag.test_and_set();
				executor_sync_flag.notify_one();
			}

			void clientWait() override
			{
				executor_sync_flag.wait(false);
			}
		};

		auto updateActiveSysTask = [&]() -> bool
		{
			GetTaskInterface sync_interface;

			TaskSync.wait(&sync_interface);
			sync_interface.clientWait();

			active_task_handle = sync_interface.task_handle;

			return ( nullptr != active_task_handle );
		};

		while ( updateActiveSysTask() )
		{
			CurrentTask = active_task_handle.promise().mParent;
			active_task_handle.resume();

			Task* last_task = CurrentTask;
			CurrentTask = nullptr;

			TaskSync.trigger(
				[&]()
				{
					if ( active_task_handle.done() )
					{
						last_task->mFinished.setValue(true);
					}
					else
					{
						BlockedTaskQueue.insert(active_task_handle);
						active_task_handle = nullptr;
					}

					return ReadyTaskQueue.size();
				}
			);
		}

		active_task_handle = nullptr;
		executor_sync_flag.clear();
	}

	bool ThreadPool::isActive()
	{
		return (nullptr != active_task_handle);
	}

	ThreadPoolSync::ThreadPoolSync()
	{
	}

	ThreadPoolSync::~ThreadPoolSync()
	{
	}
	
	void ThreadPoolSync::markForSuspend()
	{
	}
	
	void ThreadPoolSync::wake()
	{
	};

	void ThreadPoolSync::clientWait()
	{
	}
}