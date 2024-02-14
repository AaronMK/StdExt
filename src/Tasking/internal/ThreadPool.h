#ifndef _STD_EXT_SYS_TASKING_H_
#define _STD_EXT_SYS_TASKING_H_

#include <StdExt/Tasking/SyncInterface.h>
#include <StdExt/Tasking/SyncPoint.h>

#include <StdExt/Tasking/SysTask.h>

#include <queue>
#include <set>

namespace StdExt::Tasking
{
	class Task;

	class ThreadPool
	{
	public:
		static SyncPoint          TaskSync;
		static thread_local Task* CurrentTask;

		static std::queue<SysTask::handle_t> ReadyTaskQueue;
		static std::set<SysTask::handle_t>   BlockedTaskQueue;

		static const uint32_t       MaxConcurrency;
		static std::atomic<int>     ActiveExecutorCount;

		static void addExecuter();

		/**
		 * @brief
		 *  Returns true if the current context is inside the thread pool.
		 */
		static bool isActive();
	};

	class ThreadPoolSync : public SyncTasking
	{
	public:
		ThreadPoolSync();
		virtual ~ThreadPoolSync();

		void markForSuspend() override;
		void wake() override;

		void clientWait() override;
	};


}

#endif // !_STD_EXT_SYS_TASK_INTERNAL_H_