#ifndef _STD_EXT_CONCURRENT_TASK_LOOP_H_
#define _STD_EXT_CONCURRENT_TASK_LOOP_H_

#include "Condition.h"
#include "Queue.h"
#include "Task.h"

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  A task implementation that runs other tasks in sequential order.
	 * 
	 *  Being a task itself, it must be run using standard task running
	 *  mechanisms. This can either before or after adding tasks, and tasks
	 *  can be added anytime before calling end(), whether running or not.
	 *  Running a TaskLoop itself with runInline() will only be allowed
	 *  if end() has been called.
	 * 
	 *  It is up to client code to call end(), and if not run inline, to
	 *  also wait() for the loop to complete.  Failure to do so is
	 *  undefined behavior.
	 * 
	 * @code{.cpp}
	 *  class SeqTask : public Task
	 *  {
	 *  protected:
	 *    void run() override
	 *    {
	 *      // Do task work
	 *    }
	 *  };
	 *  
	 *  void foo()
	 *  {
	 *    {
	 *      TaskLoop loop;
	 *      SeqTask task1;
	 *      SeqTask task2;
	 *      
	 *      loop.add(&task1);
	 *      loop.runAsync();
	 *      
	 *      loop.add(&task2);
	 *      loop.end();
	 *      loop.wait();
	 *      
	 *      // Valid
	 *    }
	 * 
	 *    {
	 *      TaskLoop loop;
	 *      SeqTask task1;
	 *      SeqTask task2;
	 *      
	 *      loop.add(&task1);
	 *      loop.add(&task2);
	 *      loop.end();
	 * 
	 *      loop.runInline();
	 *      
	 *      // Valid
	 *    }
	 * 
	 *    {
	 *      TaskLoop loop;
	 *      SeqTask task1;
	 *      SeqTask task2;
	 *      
	 *      loop.add(&task1);
	 *      loop.add(&task2);
	 *      
	 *      // undefined behavior since end() was not
	 *      // called before loop destruction.
	 *    }
	 *    
	 *    {
	 *      TaskLoop loop;
	 *      SeqTask task1;
	 *      SeqTask task2;
	 *      
	 *      loop.add(&task1);
	 *      loop.runInline();  // Exception since end() has not been called.
	 *      loop.add(&task2);
	 *      
	 *      
	 *    }
	 *    
	 *    {
	 *      TaskLoop loop;
	 *      SeqTask task1;
	 *      SeqTask task2;
	 *      
	 *      loop.add(&task1);
	 *      loop.runAsync();
	 *      loop.end()
	 *      
	 *      loop.add(&task2); // Exception since attempting to add a task after end().
	 *    }
	 *  }
	 * @endcode
	 */
	class TaskLoop : public Task<void>
	{
	public:
		TaskLoop();
		virtual ~TaskLoop();

		/**
		 * @brief
		 *  Adds a task to the loop.  The task will be considered running after this
		 *  call up until it and its sub-tasks have been completed by the loop.
		 */
		void add(Task* task);

		/**
		 * @brief
		 *  This will cause the loop end after it has completed all added tasks.  Once finished
		 *  this object can be reused as if new.
		 */
		void end();

	protected:
		bool canInline() const override;
		void run() override;

	private:
		Queue<Task*> mQueue;
		Condition mAvailable;
		std::atomic<bool> mEnd = false;
	};
}

#endif // !_STD_EXT_CONCURRENT_TASK_LOOP_H_