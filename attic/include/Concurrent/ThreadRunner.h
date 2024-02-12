#ifndef _STD_EXT_CUNCURRENT_THREAD_RUNNER_H_
#define _STD_EXT_CUNCURRENT_THREAD_RUNNER_H_

#include "Concurrent.h"
#include "SyncPoint.h"

#include <atomic>
#include <thread>

namespace StdExt::Concurrent
{
	class TaskBase;

	/**
	 * @brief
	 *  Runs a single task on its own dedicated thread.  When the task is waiting,
	 *  the thread will block.
	 */
	class STD_EXT_EXPORT ThreadRunner final
	{
	private:
		static thread_local ThreadRunner* mActiveRunner;

		std::atomic_flag mFlag;

		TaskBase*   mTask;
		std::thread mThread;

	public:
		/**
		 * @brief
		 *  Returns true if the active context is inside a %ThreadRunner.
		 */
		static bool isActive();
		
		/**
		 * @brief
		 *  An adapter to a SyncPoint for the currently active ThreadRunner.  It will use the 
		 *  ThreadRunner's atomic flag to mark when the thread should continue.  Client code
		 *  must call wait() to block util the SyncPoint has set the flag again.
		 * 
		 * @details
		 *  - It is not valid to create an instance of this object outside the context of a
		 *    ThreadRunner (when ThreadRunner::isActive() returns false).
		 *
		 *  - Only one instance of this class should be active within any given thread.
		 */
		class ThreadSync : public SyncTasking
		{
		private:
			std::atomic_flag& mAtomicFlag;

		public:
			ThreadSync();
			virtual ~ThreadSync();

			void markForSuspend() override;
			void wake() override;

			void wait();
		};

		ThreadRunner(TaskBase* parent);
		~ThreadRunner();

		void wait();
	};
}

#endif // !_STD_EXT_CUNCURRENT_THREAD_RUNNER_H_