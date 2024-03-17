#ifndef _STD_EXT_SYS_TASKING_H_
#define _STD_EXT_SYS_TASKING_H_

#include <StdExt/Tasking/SyncInterface.h>
#include <StdExt/Tasking/SyncPoint.h>

#include <StdExt/Tasking/SysTask.h>

#include <queue>

namespace StdExt::Tasking
{
	class Task;

	class ThreadPool
	{
	public:
		static SyncPoint          TaskSync;

		static void addTask(Task* task);
		static void executerMain();

		/**
		 * @brief
		 *  Returns true if the current context is inside the thread pool.
		 */
		static bool isActive();
	};

	class ThreadPoolSync : public AtomicTaskSync
	{
	public:
		ThreadPoolSync();
		virtual ~ThreadPoolSync();

		void suspend() override;
		void wake() override;

	private:
		bool mInThreadPool;
	};

	class ThreadPoolSyncAwaiter : public SyncAwaiter
	{
	public:
		ThreadPoolSyncAwaiter(SyncPoint* sync_point, ThreadPoolSync* sync_interface);
		bool await_suspend(std::coroutine_handle<> handle) final;
	};
}

#endif // !_STD_EXT_SYS_TASK_INTERNAL_H_