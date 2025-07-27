#ifndef _STD_EXT_CONCURRENT_RW_LOCK_H_
#define _STD_EXT_CONCURRENT_RW_LOCK_H_

#include "../StdExt.h"

#ifdef _WIN32
#	include <concrt.h>
#else
#	include <pthread.h>
#endif


namespace StdExt::Concurrent
{
	class STD_EXT_EXPORT RWLock final
	{
		friend class ReadLocker;
		friend class WriteLocker;

	private:
	#ifdef _WIN32
		Concurrency::reader_writer_lock mSysLock;
	#else
		pthread_rwlock_t rwlock;
	#endif

	public:
		RWLock(const RWLock&) = delete;
		RWLock(RWLock&&) = delete;

		RWLock();
		~RWLock();

		void lockRead();
		void lockWrite();

		void unlock();
	};

	class STD_EXT_EXPORT ReadLocker
	{
	private:
	#ifdef _WIN32
		Concurrency::reader_writer_lock::scoped_lock_read mSysLocker;
	#else
		RWLock* mLock;
	#endif

	public:
		ReadLocker(RWLock& lock);
		~ReadLocker();
	};

	class STD_EXT_EXPORT WriteLocker
	{
	private:
	#ifdef _WIN32
		Concurrency::reader_writer_lock::scoped_lock mSysLocker;
	#else
		RWLock* mLock;
	#endif

	public:
		WriteLocker(RWLock& lock);
		~WriteLocker();
	};

}

#endif // !_STD_EXT_CONCURRENT_RW_LOCK_H_