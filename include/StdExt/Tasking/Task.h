#ifndef _STD_EXT_TASKING_TASK_H_
#define _STD_EXT_TASKING_TASK_H_

#include "SysTask.h"

#include <initializer_list>

#include <StdExt/Signals/Settable.h>

namespace StdExt::Tasking
{
	/**
	 * @brief
	 *  The running state of a task.
	 */
	enum class TaskState
	{
		/**
		 * @brief
		 *  The task has not been run nor is scheduled to be run.
		 */
		Dormant,

		/**
		 * @brief
		 *  The task has been placed into a scheduler, but has not stated execution.
		 */
		InQueue,

		/**
		 * @brief
		 *  The task is waiting on a timer or a condition.
		 */
		Blocked,

		/**
		 * @brief
		 *  The task is running.
		 */
		Running,

		/**
		 * @brief
		 *  The task has finished, and the results or exceptions thrown are ready.
		 */
		Finished
	};

	class STD_EXT_EXPORT Task
	{
		friend class ThreadPool;
		friend class ThreadPoolSync;

	public:
		Task();
		virtual ~Task();

		Task(const Task&) = delete;
		Task(Task&&)      = delete;

		Task& operator=(const Task&) = delete;
		Task& operator=(Task&&)      = delete;

		void start();

		static size_t waitForAny(std::initializer_list<Task*> task_list);
		static void   waitForAll(std::initializer_list<Task*> task_list);

	protected:
		virtual void run_task() = 0;

		WaitState sync(SyncPoint& sync_point, const CallableArg<bool>& predicate);
		WaitState sync(SyncPoint& sync_point, const CallableArg<bool>& predicate, const CallableArg<void, WaitState>& handler);

	private:
		TaskState               mState;
		Signals::Settable<bool> mFinished;
	};
}

#endif // !_STD_EXT_TASKING_TASK_H_