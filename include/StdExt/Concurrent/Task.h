#ifndef _STD_EXT_CONCURRENT_TASK_H_
#define _STD_EXT_CONCURRENT_TASK_H_

#include "../StdExt.h"

#include "Condition.h"

#include <atomic>
#include <functional>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Abstract task for concurrent operations.
	 * 
	 *  Tasks are the basic unit of work items that are passed into the system threadpool
	 *  via a Scheduler for execution.  They can be tracked, and subtasks for seperate
	 *  concurrent execution can be added as child tasks from within an implementation
	 *  of run().  It is the responsibility of client code to prevent the destruction
	 *  of tasks while they are running.
	 */
	class STD_EXT_EXPORT Task
	{
	private:
		Task* mParentTask;
		std::atomic<int> mDependentCount;
		Condition mFinishedHandle;
		bool mDeleteOnComplete;

		static void runTask(void*);

	public:
		Task(const Task&) = delete;
		Task(Task&&) = delete;

		Task();
		virtual ~Task();

		/**
		 * @brief
		 *  The task is either currently running, or is in the queue waiting to run.
		 */
		bool isRunning() const;

		/**
		 * @brief
		 *  Waits for the task and its subtasks to complete.  This will return immediately if the
		 *  task has not been passed to the scheduler.
		 */
		void wait();

		/**
		 * @brief
		 *  Runs that task in the current context instantly.  Any subtasks created will still go
		 *  go into the threadpool, and this task will not be considered complete until those
		 *  finish.  This will return after the main task is completed, and wait() can then be used
		 *  to track substasks.
		 */
		void runInline();

		/**
		 * @brief
		 *  Submits the task to the threadpool for execution, and puts the task into the running state.
		 */
		void runAsync();

		/**
		 * Runs the task in its own thread.  Subtasks will be sent to the threadpool.
		 */
		void runAsThread();

	protected:

		/**
		 * @brief
		 *  Override to determine what your task will do.
		 */
		virtual void run() = 0;

		/**
		 * @brief
		 *  Submits task to the threadpool as a subtask of this task.  This can only be done
		 *  when this task is active.  Subtasks are must complete before the parent task is
		 *  considered complete, and are submitted to a higher priority queue in the
		 *  threadpool.
		 */
		void subtask(Task* task);

		/**
		 * @brief
		 *  Runs func as a subtask of the current task.
		 */
		void subtask(std::function<void()>&& func);

		/**
		 * @brief
		 *  Runs func as a subtask of the current task.
		 */
		void subtask(const std::function<void()>& func);
	};
}

#endif // !_STD_EXT_CONCURRENT_TASK_H_