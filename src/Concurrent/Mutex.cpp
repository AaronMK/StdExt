#include <StdExt/Concurrent/Mutex.h>
#include <StdExt/Concurrent/ThreadRunner.h>

namespace StdExt::Concurrent
{
#if defined(STD_EXT_COROUTINE_TASKS) || defined(STD_EXT_APPLE)
	namespace details
	{
		MutexSync::MutexSync(Mutex* mutex)
			: mMutex(mutex)
		{
		}

		MutexSync::~MutexSync()
		{
			mMutex->trigger(
				[this]() -> size_t
				{
					if (mMutex->mLocked)
					{
						mMutex->mLocked = false;
						return 1;
					}

					return 0;
				}
			);
		}

		bool MutexSync::testPredicate()
		{
			return (false == mMutex->mLocked);
		}

		void MutexSync::atomicAction()
		{
			mMutex->mLocked = true;
		}
	}

	Mutex::Mutex()
	{
	}

	Mutex::~Mutex()
	{
	}

	void Mutex::lock()
	{
		if ( ThreadRunner::isActive() )
		{
			using tr_sync_t = ThreadRunner::SyncBase<details::MutexSync>;

			class LocalSync : public tr_sync_t
			{
			public:
				LocalSync(Mutex* mutex) : tr_sync_t(mutex) {}
				virtual ~LocalSync() {}
			};

			LocalSync sync(this);
			wait(&sync);

			sync.wait();
		}
	}

	void Mutex::unlock()
	{
		trigger(
			[&]()
			{
				this->mLocked = false;
				return true;
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