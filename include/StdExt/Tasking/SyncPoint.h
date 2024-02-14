#ifndef _STD_EXT_CONCURRENCY_SYNC_POINT_H_
#define _STD_EXT_CONCURRENCY_SYNC_POINT_H_

#include "SyncInterface.h"

#include "../Callable.h"
#include "../CallableTraits.h"
#include "../Concepts.h"

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
	public:
		static constexpr auto NO_INDEX = SyncInterface::NO_INDEX;

		class Awaiter
		{
		private:
			std::unique_lock<std::mutex> mLock;
			SyncInterface *mSyncInt;
			SyncPoint *mSyncPt;

		public:
			Awaiter(SyncInterface *sync_int, SyncPoint *sync_pt)
			{
				mSyncInt = sync_int;
				mSyncPt = sync_pt;
				mLock = std::unique_lock(mSyncPt->mMutex);
			}

			bool await_ready()
			{
				if (mSyncInt->testPredicate())
				{
					mSyncInt->wait_state = WaitState::PredicateSatisfied;
					mSyncInt->atomicAction(mSyncInt->wait_state);
					mLock.unlock();

					return true;
				}

				mSyncInt->wait_state = WaitState::Waiting;
				mSyncPt->mWaiters.emplace_back(mSyncInt);

				return false;
			}

			void await_suspend(std::coroutine_handle<> handle)
			{
				mSyncInt->markForSuspend();
				mLock.unlock();
			}

			WaitState await_resume()
			{
				return mSyncInt->wait_state;
			}
		};

		SyncPoint();
		virtual ~SyncPoint();

		void wait(SyncInterface *waiter);
		bool cancel(SyncInterface *sync_item);

		/**
		 * @brief
		 *  Waits for the predicate to be satisfied.  The synchronization mechanism is determined within the call.
		 *  If client code is running within the context of a Task, the cooperative mechanisms specified in that
		 *  task will be used.  Otherwise, the underlying thread will wait by sleeping.
		 * 
		 * @param predicate
		 *  Routine to test if the predicate condition has been met.  Called in the context of trigger() calls
		 *  to this %SyncPoint.
		 */
		WaitState wait(const CallableArg<bool>& predicate);
		WaitState wait(const CallableArg<bool>& predicate, const CallableArg<void, WaitState>& handler);


		void callAtomic(const CallableArg<void>& func);

		void trigger(const CallableArg<void>& trigger_func);
		void trigger(const CallableArg<bool>& trigger_func);
		void trigger(const CallableArg<size_t>& trigger_func);

		void destroy();

		auto await(SyncInterface *sync)
		{
			return Awaiter(sync, this);
		}

	private:
		void wakeReady(size_t max_count);
;
		Collections::Vector<SyncInterface *, 4, false, 16> mWaiters;
		std::mutex mMutex;
		bool mDestroyed;

	};
}

#endif // !_STD_EXT_CONCURRENCY_SYNC_POINT_H_