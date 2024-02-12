#include <StdExt/Concurrent/Mutex.h>
#include <StdExt/Concurrent/ThreadRunner.h>

namespace StdExt::Concurrent
{
#if defined(STD_EXT_COROUTINE_TASKS) || defined(STD_EXT_APPLE)
	class MutexActions final : public SyncActions
	{
	private:
		Mutex* mMutex;

	public:
		MutexActions(Mutex* mutex)
			: mMutex(mutex)
		{
		}

		virtual ~MutexActions()
		{
		}
		
		bool testPredicate() override
		{
			return (false == mMutex->mLocked);
		}

		void atomicAction() override
		{
			mMutex->mLocked = true;
		}
	};

	Mutex::Mutex()
	{
		mLocked = false;
	}

	Mutex::~Mutex()
	{
	}

	void Mutex::lock()
	{
		if ( ThreadRunner::isActive() )
		{
			CombinedSyncInterface sync(MutexActions(this), ThreadRunner::ThreadSync());
			wait(&sync);

			sync.Tasking.wait();
			auto ws = sync.waitState();

			if ( SyncPoint::WaitState::Destroyed == ws )
				throw object_destroyed();
			else if (SyncPoint::WaitState::Complete != ws )
				throw unknown_error("Concurrent Mutex Failed.");
		}

		throw not_implemented();
	}

	void Mutex::unlock()
	{
		trigger(
			[&]() -> size_t
			{
				assert(mLocked);
				
				mLocked = false;
				return 1;
			}
		);
	}

#elif defined(STD_EXT_WIN32)

	bool Mutex::lock()
	{
		try 
		{
			mPlatformData.cs.lock();
		}
		catch (const Concurrency::improper_lock&)
		{
		}

		mPlatformData.entryCount++;
		return true;
	}

	void Mutex::unlock()
	{
		if ( 0 == --mPlatformData.entryCount )
			mPlatformData.cs.unlock();
	}

#else

	bool Mutex::lock()
	{
		mMutex.lock();
		return true;
	}

	void Mutex::unlock()
	{
		mMutex.unlock();
	}

#endif
}