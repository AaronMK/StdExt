#ifndef _STD_EXT_CONCURRENCY_SYNC_POINT_H_
#define _STD_EXT_CONCURRENCY_SYNC_POINT_H_

#include "Concurrent.h"

#include "../Collections/Vector.h"
#include "../Concepts.h"
#include "../CallableTraits.h"

#include <cassert>
#include <limits>
#include <mutex>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Interface used by a SyncPoint to interact with client code.
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
			 *  Precondition has not been met and the item has not timed out.
			 */
			Waiting,

			/**
			 * @brief
			 *  Precondition has been met and atomicAction() has executed.
			 */
			Complete,

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

		static constexpr uint32_t NO_INDEX = std::numeric_limits<uint32_t>::max();

		SyncInterface()
		{
			uint32_t  wait_index = NO_INDEX;
			WaitState wait_state = WaitState::None;
		}

		virtual ~SyncInterface()
		{
		}

		uint32_t waitIndex() const
		{
			return wait_index;
		}

		WaitState waitState()
		{
			return wait_state;
		}

	protected:
		virtual bool testPredicate()  = 0;
		virtual void atomicAction()   = 0;
		virtual void markForSuspend() = 0;
		virtual void wake()           = 0;

	private:
		uint32_t  wait_index;
		WaitState wait_state;
	};

	class SyncPoint
	{
	public:
		using WaitState                = SyncInterface::WaitState;
		static constexpr auto NO_INDEX = SyncInterface::NO_INDEX;

		class Awaiter
		{
		private:
			std::unique_lock<std::mutex> mLock;
			SyncInterface*               mSyncInt;
			SyncPoint*                   mSyncPt;

		public:
			Awaiter(SyncInterface* sync_int, SyncPoint* sync_pt)
			{
				mSyncInt = sync_int;
				mSyncPt  = sync_pt;
				mLock = std::unique_lock(mSyncPt->mMutex);
			}

			bool await_ready()
			{
				if ( mSyncInt->testPredicate() )
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

		SyncPoint()
		{
			mDestroyed = false;
		}

		virtual ~SyncPoint()
		{
			destroy();
		}

		void wait(SyncInterface* waiter)
		{
			assert( WaitState::None == waiter->wait_state);

			std::unique_lock lock(mMutex);

			if ( mDestroyed )
			{
				waiter->wait_state = WaitState::Destroyed;
				return;
			}

			if ( waiter->testPredicate() )
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

		bool cancel(SyncInterface* sync_item)
		{
			std::unique_lock lock(mMutex);

			if ( sync_item->wait_index != NO_INDEX )
			{
				assert( sync_item->wait_state == WaitState::Waiting );
				assert( mWaiters[sync_item->wait_index] == sync_item );

				uint32_t vacant_index = sync_item->wait_index;
				sync_item->wait_index = NO_INDEX;
				sync_item->wait_state = WaitState::Canceled;
				sync_item->wake();
				
				mWaiters[vacant_index] = nullptr;

				for(uint32_t idx = vacant_index + 1; idx < mWaiters.size(); ++idx)
				{
					mWaiters[idx]->wait_index = idx - 1;
					mWaiters[idx - 1]         = mWaiters[idx];
					mWaiters[idx]             = nullptr;
				}

				mWaiters.resize(mWaiters.size() - 1);

				return true;
			}

			return false;
		}

		template<typename trigger_t>
			requires std::is_same_v<typename CallableTraits<trigger_t>::return_t, uint32_t>
		void trigger(const trigger_t &trigger_func)
		{
			std::unique_lock lock(mMutex);
			uint32_t wake_max = trigger_func();

			if ( wake_max > 0 )
				wakeReady(wake_max);
		}

		template<ReturnsType<bool> trigger_t>
			requires std::is_same_v<typename CallableTraits<trigger_t>::return_t, bool>
		void trigger(const trigger_t &trigger_func)
		{
			std::unique_lock lock(mMutex);
			
			if ( trigger_func() )
				wakeReady( std::numeric_limits<uint32_t>::max() );
		}

		auto await(SyncInterface* sync)
		{
			return Awaiter(sync, this);
		}

		void destroy()
		{
			std::unique_lock lock(mMutex);

			for(size_t idx = 0; idx < mWaiters.size(); ++idx)
			{
				SyncInterface* sync = mWaiters[idx];
				sync->wait_state = WaitState::Destroyed;
				sync->wait_index = NO_INDEX;
				sync->wake();

				mWaiters[idx] = nullptr;
			}

			mWaiters.resize(0);
			mDestroyed = true;
		}

	private:
		void wakeReady(uint32_t max_count)
		{
			uint32_t last_null_idx     = NO_INDEX;
			uint32_t remaining_waiters = 0;
			uint32_t wake_count        = 0;

			for(uint32_t idx = 0; idx < mWaiters.size(); ++idx)
			{
				auto waiter = mWaiters[idx];

				if ( wake_count < max_count && waiter->testPredicate()  )
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
					if ( NO_INDEX != last_null_idx )
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
		bool       mDestroyed;

		Collections::Vector<SyncInterface*, 4, false, 16> mWaiters;
	};
}

#endif // !_STD_EXT_CONCURRENCY_SYNC_POINT_H_