#ifndef _STD_EXT_TASKING_TASK_H_
#define _STD_EXT_TASKING_TASK_H_

#include "SyncPoint.h"
#include "SysTask.h"

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
		 *  Task has finished, and the results or exceptions thrown are ready.
		 */
		Finished
	};

	class STD_EXT_EXPORT Task
	{
	public:
		Task();
		virtual ~Task();

	protected:
		virtual void run_task() = 0;

	private:
		std::optional<SysTask> mRunner;
	};
}

#endif // !_STD_EXT_TASKING_TASK_H_