#include <StdExt/Tasking/SyncPoint.h>

#include "internal/ThreadPool.h"

namespace StdExt::Tasking
{
	SyncPoint::SyncPoint()
	{
		mDestroyed = false;
	}

	SyncPoint::~SyncPoint()
	{
		destroy();
	}

	WaitState SyncPoint::wait(SyncInterface* waiter)
	{
		assert(WaitState::None == waiter->wait_state);

		std::unique_lock lock(mMutex);
		handleWaitLogic(waiter);

		return waiter->wait_state;
	}

	bool SyncPoint::cancel(SyncInterface *sync_item)
	{
		std::unique_lock lock(mMutex);

		if (sync_item->wait_index != NO_INDEX)
		{
			assert(sync_item->wait_state == WaitState::Waiting);
			assert(mWaiters[sync_item->wait_index] == sync_item);

			size_t vacant_index = sync_item->wait_index;
			sync_item->wait_index = NO_INDEX;
			sync_item->wait_state = WaitState::Canceled;
			sync_item->atomicAction(sync_item->wait_state);
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

	void SyncPoint::protectedAction(const CallableArg<void>& action_func)
	{
		std::unique_lock lock(mMutex);
		action_func();
	}

	void SyncPoint::trigger(const CallableArg<void>& trigger_func)
	{
		std::unique_lock lock(mMutex);

		trigger_func();
		wakeReady(std::numeric_limits<uint32_t>::max());
	}

	void SyncPoint::trigger(const CallableArg<bool>& trigger_func)
	{
		std::unique_lock lock(mMutex);

		if (trigger_func())
			wakeReady(std::numeric_limits<uint32_t>::max());
	}

	void SyncPoint::trigger(const CallableArg<size_t>& trigger_func)
	{
		std::unique_lock lock(mMutex);
		size_t wake_count = trigger_func();

		if ( wake_count > 0 )
			wakeReady(wake_count);
	}

	void SyncPoint::destroy()
	{
		std::unique_lock lock(mMutex);

		for (size_t idx = 0; idx < mWaiters.size(); ++idx)
		{
			SyncInterface *sync = mWaiters[idx];
			mWaiters[idx] = nullptr;

			sync->wait_state = WaitState::Destroyed;
			sync->wait_index = NO_INDEX;
			sync->atomicAction(sync->wait_state);
			sync->wake();

		}

		mWaiters.resize(0);
		mDestroyed = true;
	}

	void SyncPoint::handleWaitLogic(SyncInterface* waiter)
	{
		if (mDestroyed)
		{
			waiter->wait_state = WaitState::Destroyed;
			return;
		}

		waiter->initialize();

		if (waiter->testPredicate())
		{
			waiter->wait_state = WaitState::PredicateSatisfied;
			waiter->atomicAction( waiter->wait_state );
		}
		else
		{
			mWaiters.emplace_back(waiter);
			waiter->wait_state = WaitState::Waiting;
			waiter->wait_index = mWaiters.size() - 1;
			waiter->suspend();
		}
	}
	
	void SyncPoint::wakeReady(size_t max_count)
	{
		size_t last_null_idx = NO_INDEX;
		size_t remaining_waiters = 0;
		size_t wake_count = 0;

		for (size_t idx = 0; idx < mWaiters.size(); ++idx)
		{
			auto waiter = mWaiters[idx];

			if (wake_count < max_count && waiter->testPredicate())
			{
				waiter->wait_state = WaitState::PredicateSatisfied;
				waiter->wait_index = NO_INDEX;
				waiter->atomicAction(waiter->wait_state);
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

	SyncAwaiter::SyncAwaiter(SyncPoint* sync_point, SyncInterface* sync_interface)
		: mSyncPoint(sync_point), mWaiter(sync_interface),
		  mLock(mSyncPoint->mMutex), mWaitState(WaitState::None)
	{
	}
	
	SyncAwaiter::~SyncAwaiter()
	{
	}

	bool SyncAwaiter::await_ready()
	{
		mSyncPoint->handleWaitLogic(mWaiter);

		switch ( mWaiter->wait_state )
		{
		case WaitState::Waiting:
			return false;
		default:
			return true;
		}
	}

	WaitState SyncAwaiter::await_resume()
	{
		return mWaiter->wait_state;
	}

	SyncPoint* SyncAwaiter::getSyncPoint()
	{
		return mSyncPoint;
	}

	SyncInterface* SyncAwaiter::getSyncInterface()
	{
		return mWaiter;
	}
}