#include <StdExt/Concurrent/RWLock.h>

#include <StdExt/Exceptions.h>

namespace StdExt::Concurrent
{

#ifdef _WIN32
	static const char* multi_lock_msg =
		"Attempting to obtain lock already held by the current context.";


	RWLock::RWLock()
	{
	}

	RWLock::~RWLock()
	{
	}

	void RWLock::lockRead()
	{
		try
		{
			mSysLock.lock_read();
		}
		catch ( const Concurrency::improper_lock& )
		{
			throw invalid_operation(multi_lock_msg);
		}
	}

	void RWLock::lockWrite()
	{
		try
		{
			mSysLock.lock();
		}
		catch ( const Concurrency::improper_lock& )
		{
			throw invalid_operation(multi_lock_msg);
		}
	}
	
	void RWLock::unlock()
	{
		mSysLock.unlock();
	}

	/////////////////////////////

	ReadLocker::ReadLocker(RWLock& lock)
		: mSysLocker(lock.mSysLock)
	{

	}

	ReadLocker::~ReadLocker()
	{

	}

	/////////////////////////////

	WriteLocker::WriteLocker(RWLock& lock)
		: mSysLocker(lock.mSysLock)
	{

	}

	WriteLocker::~WriteLocker()
	{

	}
#else

	RWLock::RWLock()
	{
		if (0 != pthread_rwlock_init(&rwlock, nullptr) )
			throw std::runtime_error("Failed to create read/write lock");
	}

	RWLock::~RWLock()
	{
		pthread_rwlock_destroy(&rwlock);
	}

	void RWLock::lockRead()
	{
		pthread_rwlock_rdlock(&rwlock);
	}

	void RWLock::lockWrite()
	{
		pthread_rwlock_wrlock(&rwlock);
	}
	
	void RWLock::unlock()
	{
		pthread_rwlock_unlock(&rwlock);
	}

	/////////////////////////////

	ReadLocker::ReadLocker(RWLock& lock)
		: mLock(&lock)
	{
		mLock->lockRead();
	}

	ReadLocker::~ReadLocker()
	{
		mLock->unlock();
	}

	/////////////////////////////

	WriteLocker::WriteLocker(RWLock& lock)
		: mLock(&lock)
	{
		mLock->lockWrite();
	}

	WriteLocker::~WriteLocker()
	{
		mLock->unlock();
	}

#endif
}