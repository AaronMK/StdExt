#include <StdExt/Concurrent/RWLock.h>

#include <StdExt/Exceptions.h>

namespace StdExt::Concurrent
{
	static const char* mulit_lock_msg = 
		"Attempting to obtain lock already held by the current context.";

#ifdef _WIN32

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
			throw invalid_operation(mulit_lock_msg);
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
			throw invalid_operation(mulit_lock_msg);
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

#endif
}