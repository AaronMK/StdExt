#include <StdExt/Concurrent/Mutex.h>
#include "..\..\include\StdExt\Concurrent\Mutex.h"


namespace StdExt::Concurrent
{

#	ifdef _WIN32

	Mutex::Mutex()
	{
	}

	Mutex::~Mutex()
	{
	}

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

#	endif

	MutexLocker::MutexLocker(Mutex& M)
		: mMutex(&M)
	{
		mMutex->lock();
	}

	MutexLocker::MutexLocker(MutexLocker&& other) noexcept
	{
		mMutex = other.mMutex;
		other.mMutex = nullptr;
	}

	MutexLocker::~MutexLocker()
	{
		if ( mMutex )
			mMutex->unlock();
	}

	MutexLocker& MutexLocker::operator=(MutexLocker&& other)
	{
		mMutex = other.mMutex;
		other.mMutex = nullptr;

		return *this;
	}
}