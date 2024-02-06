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
	 *  Runs a task on its own dedicated thread.  When the task is waiting,
	 *  the thread will block.
	 */
	class ThreadRunner final
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

		template<SubclassOf<SyncInterface> base_sync_t = SyncInterface>
		class SyncBase : public base_sync_t
		{
		private:
			std::atomic_flag& mAtomicFlag;

		public:
			template<typename... base_args_t>
			SyncBase(base_args_t... args)
				: base_sync_t(std::forward<base_args_t>(args)...),
				  mAtomicFlag(mActiveRunner->mFlag)
			{
				mAtomicFlag.test_and_set();
			};

			virtual ~SyncBase()
			{
			}

			void markForSuspend() final
			{
				mAtomicFlag.clear();
			}
			
			void wake() final
			{
				mAtomicFlag.test_and_set();
				mAtomicFlag.notify_one();
			}

			void wait()
			{
				mAtomicFlag.wait(true);
			}
		};

		ThreadRunner(TaskBase* parent);
		virtual ~ThreadRunner();
	};
}

#endif // !_STD_EXT_CUNCURRENT_THREAD_RUNNER_H_