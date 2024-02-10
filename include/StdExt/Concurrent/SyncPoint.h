#ifndef _STD_EXT_CONCURRENCY_SYNC_POINT_H_
#define _STD_EXT_CONCURRENCY_SYNC_POINT_H_

#include "Concurrent.h"

#include "../Concepts.h"
#include "../Collections/Vector.h"

#include <limits>
#include <mutex>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Interface used by a SyncPoint to interact with client code.
	 * 
	 * @details
	 *  This interface brings the task management and program logic into a SyncPoint
	 *  for processing and callback interaction.  Implementations can be created
	 *  directly, but it is usually a tasking system that will determine the
	 *  implmentations of markForSuspend() and wake(), and more generic program
	 *  logic that will implement testPredicate() and atomicAction().
	 * 
	 *  There are utility templates and classes to compose the tasking and program
	 *  logic seperately and combine them into a single %SyncInterface implmentation
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
		 *  Called when the precondition (as determined by testPredicate()) is not met and a wait needs to
		 *  occur.  An implmentation of this function should arrange for the calling context to be
		 *  suspended.  Any implementation should <b>NOT</b> block, or a deadlock could result.
		 */
		virtual void markForSuspend() = 0;

		/**
		 * @brief
		 *  Called by in the context of the SyncPoint::trigger() call that creates the conditions to satisfy
		 *  a predicate.  An implementation should
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
	};

	class SyncTasking
	{
	public:
		virtual void markForSuspend() = 0;
		virtual void wake() = 0;
	};

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
					return true;
				}

				mSyncPt->mWaiters.emplace_back(mSyncInt);
				return false;
			}

			void await_suspend(std::coroutine_handle<> handle)
			{
				mSyncInt->markForSuspend();
			}

			void await_resume()
			{
				mLock.unlock();
			}
		};

		SyncPoint();

		virtual ~SyncPoint();

		void wait(SyncInterface *waiter)
		{
			assert(WaitState::None == waiter->wait_state);

			std::unique_lock lock(mMutex);

			if (mDestroyed)
			{
				waiter->wait_state = WaitState::Destroyed;
				return;
			}

			if (waiter->testPredicate())
			{
				waiter->atomicAction();
			}
			else
			{
				mWaiters.emplace_back(waiter);
				waiter->wait_state = WaitState::Waiting;
				waiter->wait_index = mWaiters.size() - 1;
				waiter->markForSuspend();
			}
		}

		bool cancel(SyncInterface *sync_item)
		{
			std::unique_lock lock(mMutex);

			if (sync_item->wait_index != NO_INDEX)
			{
				assert(sync_item->wait_state == WaitState::Waiting);
				assert(mWaiters[sync_item->wait_index] == sync_item);

				size_t vacant_index = sync_item->wait_index;
				sync_item->wait_index = NO_INDEX;
				sync_item->wait_state = WaitState::Canceled;
				sync_item->wake();

				mWaiters[vacant_index] = nullptr;

				for (size_t idx = vacant_index + 1; idx < mWaiters.size(); ++idx)
				{
					mWaiters[idx]->wait_index = idx - 1;
					mWaiters[idx - 1] = mWaiters[idx];
					mWaiters[idx] = nullptr;
				}

				mWaiters.resize(mWaiters.size() - 1);

				return true;
			}

			return false;
		}

		template <ReturnsType<size_t> trigger_t>
		void trigger(const trigger_t &trigger_func)
		{
			std::unique_lock lock(mMutex);
			size_t wake_max = trigger_func();

			if (wake_max > 0)
				wakeReady(wake_max);
		}

		template <ReturnsType<bool> trigger_t>
		void trigger(const trigger_t &trigger_func)
		{
			std::unique_lock lock(mMutex);

			if (trigger_func())
				wakeReady(std::numeric_limits<uint32_t>::max());
		}

		template <ReturnsType<void> trigger_t>
		void trigger(const trigger_t &trigger_func)
		{
			std::unique_lock lock(mMutex);

			trigger_func();
			wakeReady(std::numeric_limits<uint32_t>::max());
		}

		auto await(SyncInterface *sync)
		{
			return Awaiter(sync, this);
		}

		void destroy()
		{
			std::unique_lock lock(mMutex);

			for (size_t idx = 0; idx < mWaiters.size(); ++idx)
			{
				SyncInterface *sync = mWaiters[idx];
				sync->wait_state = WaitState::Destroyed;
				sync->wait_index = NO_INDEX;
				sync->wake();

				mWaiters[idx] = nullptr;
			}

			mWaiters.resize(0);
			mDestroyed = true;
		}

	private:
		void wakeReady(size_t max_count)
		{
			size_t last_null_idx = NO_INDEX;
			size_t remaining_waiters = 0;
			size_t wake_count = 0;

			for (size_t idx = 0; idx < mWaiters.size(); ++idx)
			{
				auto waiter = mWaiters[idx];

				if (wake_count < max_count && waiter->testPredicate())
				{
					waiter->wait_state = WaitState::Complete;
					waiter->wait_index = NO_INDEX;
					waiter->atomicAction();
					waiter->wake();

					mWaiters[idx] = nullptr;
					last_null_idx = idx;
					++wake_count;
				}
				else
				{
					if (NO_INDEX != last_null_idx)
					{
						mWaiters[last_null_idx] = mWaiters[idx];
						mWaiters[last_null_idx]->wait_index = last_null_idx;
						mWaiters[idx] = nullptr;
						last_null_idx = idx;
					}

					++remaining_waiters;
				}
			}

			mWaiters.resize(remaining_waiters);
		}

		std::mutex mMutex;
		bool mDestroyed;

		Collections::Vector<SyncInterface *, 4, false, 16> mWaiters;
	};

	/**
	 * @brief
	 */
	class AtomicTaskSync : public SyncTasking
	{
		std::atomic_flag mFlag;

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