#ifndef _STD_EXT_CONCURRENCY_SYNC_POINT_H_
#define _STD_EXT_CONCURRENCY_SYNC_POINT_H_

#include "Tasking.h"

#include "../Concepts.h"
#include "../Callable.h"

#include "../Collections/Vector.h"

#include <limits>
#include <mutex>

namespace StdExt::Tasking
{
	/**
	 * @brief
	 *  Interface used by a SyncPoint to interact with client code.
	 * 
	 * @details
	 *  This interface brings the task management and program logic into a SyncPoint
	 *  for processing and callback interaction.  Implementations can be created
	 *  directly, but it is usually a tasking system that will determine the
	 *  implementations of markForSuspend() and wake(), and more generic program
	 *  logic that will implement testPredicate() and atomicAction().
	 * 
	 *  There are utility templates and classes to compose the tasking and program
	 *  logic separately and combine them into a single %SyncInterface implementation
	 *  that can be passed into a SyncPoint for management and processing.
	 */
	class SyncInterface
	{
		friend class SyncPoint;

	public:
		enum class WaitState
		{
			/**
			 * @brief
			 *  Initial wait state before any interaction with with a SyncPoint.
			 */
			None,

			/**
			 * @brief
			 *  Item is still waiting for the precondition to be met, and it has not been cancelled
			 *  or timed-out, nor has the SyncPoint been destroyed.
			 */
			Waiting,

			/**
			 * @brief
			 *  Precondition has been met and atomicAction() has executed.
			 */
			Complete,

			/**
			 * @brief
			 *  A wait for the predicate condition timed out.
			 */
			Timeout,

			/**
			 * @brief
			 *  Item's precondition was not met before the SyncPoint was destroyed.
			 */
			Destroyed,

			/**
			 * @brief
			 *  Item's precondition was not met before being canceled.
			 */
			Canceled
		};

		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		SyncInterface();
		virtual ~SyncInterface();

		/**
		 * @brief
		 *  Read access to the wait state.  This should be used to check and react to the state after
		 *  passing this %SyncInterface to a SyncPoint::wait() command.
		 */
		WaitState waitState() const;

	protected:
		/**
		 * @brief
		 *  Override runs a test of a predicate condition.
		 */
		virtual bool testPredicate() = 0;

		/**
		 * @brief
		 *  An action to take that needs to happen atomically with respect to other client code interacting
		 *  with a SyncPoint once a test predicate is satisfied.
		 */
		virtual void atomicAction() = 0;
		
		/**
		 * @brief
		 *  Called if  An opportunity to handle the predicate never being satisfied atomically with respect
		 *  to other clients interacting with this %SyncPoint.  The default implementation does nothing.
		 * 
		 * @param result
		 *  The state at the time the wait was abandoned.
		 */
		virtual void atomicFailHandler(WaitState result);

		/**
		 * @brief
		 *  Called when the precondition (as determined by testPredicate()) is not met and a wait needs to
		 *  occur.  An implmentation of this function should arrange for the calling context to be
		 *  suspended.  Any implementation should <b>NOT</b> block, or a deadlock could result.
		 */
		virtual void markForSuspend() = 0;

		/**
		 * @brief
		 *  Called by in the context of the SyncPoint::trigger() call that creates the conditions to satisfy
		 *  testPredicate().  A call to this function should be safe immediately after a call to markForSuspend().
		 */
		virtual void wake() = 0;

	private:
		size_t wait_index;
		WaitState wait_state;
	};

	class SyncActions
	{
	public:
		virtual bool testPredicate() = 0;
		
		virtual void atomicAction() = 0;
		virtual void atomicFailHandler(SyncInterface::WaitState result) {};
	};

	class SyncTasking
	{
	public:
		virtual void markForSuspend() = 0;
		virtual void wake() = 0;
	};

	/**
	 * @brief
	 *  Object that allows multiple threads to synchronize their execution around flexible sets of
	 *  preconditions.  Preconditions, and methods threads may use to alert waiting threads that
	 *  those conditions have been met
	 * 
	 * @note
	 *  This may be templated if profiling shows this to be a hotspot that could benefit from some devirtualization
	 *  opportunities.  However, for initial developmemnt, debugging non-templated code is easier.
	 */
	class SyncPoint
	{
	public:
		using WaitState = SyncInterface::WaitState;
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
					mSyncInt->atomicAction();
					mLock.unlock();

					return true;
				}

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

		std::mutex mMutex;
		bool mDestroyed;

		Collections::Vector<SyncInterface *, 4, false, 16> mWaiters;
	};

	/**
	 * @brief
	 */
	class AtomicTaskSync : public SyncTasking
	{
		std::atomic_flag *mFlag;

	public:
		AtomicTaskSync();
		virtual ~AtomicTaskSync();

		void markForSuspend() override;
		void wake() override;

		void waitForFlag();
	};

	template <CallableWith<bool> predicate_t, CallableWith<void> action_t>
	class CallableSyncActions : public SyncActions
	{
	private:
		predicate_t mPredicate;
		action_t mAction;

	public:
		CallableSyncActions(predicate_t &&pred, action_t &&act)
			: mPredicate(std::move(pred)), mAction(std::move(act))
		{
		}

		bool testPredicate() final
		{
			return mPredicate();
		}

		void atomicAction() final
		{
			mAction();
		}
	};

	template <SubclassOf<SyncActions> actions_t, SubclassOf<SyncTasking> tasking_t>
	class CombinedSyncInterface : public SyncInterface
	{
	public:
		actions_t Actions;
		tasking_t Tasking;

		CombinedSyncInterface(actions_t &&actions, tasking_t &&tasking)
			: Actions(std::move(actions)), Tasking(std::move(tasking))
		{
		}

		virtual ~CombinedSyncInterface()
		{
		}

		bool testPredicate() final
		{
			return Actions.testPredicate();
		}

		void atomicAction() final
		{
			Actions.atomicAction();
		}

		void markForSuspend() final
		{
			Tasking.markForSuspend();
		}

		void wake() final
		{
			Tasking.wake();
		}
	};
}

#endif // !_STD_EXT_CONCURRENCY_SYNC_POINT_H_