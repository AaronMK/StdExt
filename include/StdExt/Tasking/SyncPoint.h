#ifndef _STD_EXT_CONCURRENCY_SYNC_POINT_H_
#define _STD_EXT_CONCURRENCY_SYNC_POINT_H_

#include "SyncInterface.h"

#include "../Callable.h"

#include "../Collections/Vector.h"

#include <mutex>
#include <coroutine>

namespace StdExt::Tasking
{
	/**
	 * @brief
	 *  Object that allows multiple threads to synchronize their execution around flexible sets of
	 *  preconditions.  Preconditions, and methods threads may use to alert waiting threads that
	 *  those conditions have been met
	 * 
	 * @note
	 *  This may be templated if profiling shows this to be a hotspot that could benefit from some de-virtualization
	 *  opportunities.  However, for initial development, debugging non-templated code is easier.
	 */
	class STD_EXT_EXPORT SyncPoint
	{
		friend class SyncAwaiter;
	public:
		static constexpr auto NO_INDEX = SyncInterface::NO_INDEX;

		SyncPoint();
		virtual ~SyncPoint();

		WaitState wait(SyncInterface* waiter);
		bool cancel(SyncInterface *sync_item);

		void protectedAction(const CallableArg<void>& action_func);

		void trigger(const CallableArg<void>& trigger_func);
		void trigger(const CallableArg<bool>& trigger_func);
		void trigger(const CallableArg<size_t>& trigger_func);

		void destroy();

	private:
		void handleWaitLogic(SyncInterface* waiter);
		void wakeReady(size_t max_count);
;
		Collections::Vector<SyncInterface*, 4, false, 16> mWaiters;
		std::mutex mMutex;
		bool mDestroyed;
	};
	
	/**
	 * @brief
	 *  Adapts the logic used in a wait call to the coroutine awaiter interface.
	 * 
	 * @details
	 *  Through the lifetime of the object, a lock is held for exclusive access to the
	 *  SyncPoint.  This allows for all awaiter stages to happen atomically with respect
	 *  to other clients interacting with the SyncPoint, but means the SyncAwaiter should
	 *  be tightly scoped.
	 */
	class SyncAwaiter
	{
	public:
		SyncAwaiter(SyncPoint* sync_point, SyncInterface* sync_interface);
		virtual ~SyncAwaiter();

		/**
		 * @brief
		 *  All stages that would normally happen during SyncPoint::wait() call
		 *  happen in this stage of the awaiter.
		 * 
		 * @details
		 *  Even though there is a sperate await_suspend() method, the SyncInterface::suspend()
		 *  implementation will be called under the same circumstances as it would otherwise.
		 * 
		 * @return
		 *  True if the predicate was initially satisfied and the sync operation has completed,
		 *  or a failure reason can be determined.  False if the sync operation is put in a
		 *  waiting state.
		 */
		bool await_ready();

		/**
		 * @brief
		 *  The implementation should handle any suspension operations that are not handled
		 *  by the SyncInterface::suspend() implementation of the interface passed to the
		 *  constructor.
		 *
		 * @param handle
		 *  The handle of the coroutine being suspended.
		 * 
		 * @return
		 *  True if the coroutine should be suspended, false otherwise.
		 */
		virtual bool await_suspend(std::coroutine_handle<> handle) = 0;

		/**
		 * @brief
		 *  Returns the wait state at the end of the co_await operation.
		 */
		WaitState await_resume();

	protected:
		SyncPoint*     getSyncPoint();
		SyncInterface* getSyncInterface();

	private:
		SyncPoint*                   mSyncPoint;
		SyncInterface*               mWaiter;

		std::unique_lock<std::mutex> mLock;
		WaitState                    mWaitState;
	};
}

#endif // !_STD_EXT_CONCURRENCY_SYNC_POINT_H_