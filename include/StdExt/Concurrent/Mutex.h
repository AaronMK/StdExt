#ifndef _STD_EXT_CONCURRENT_MUTEX_H_
#define _STD_EXT_CONCURRENT_MUTEX_H_

#include "../StdExt.h"

#ifdef _WIN32
#	include <concrt.h>
#else
#	include <mutex>
#endif

namespace StdExt::Concurrent
{

	/**
	 * @brief
	 *  A mutex class that abstracts system specific cooperative implementations.
	 * 
	 *  A thread can lock a mutex multiple times, but must have a
	 *  corresponding unlock call for each lock call.  The Mutex 
	 *  is cooperative with the system thread pool.
	 *
	 *  See MutexLocker for scope based locking.
	 */
	class STD_EXT_EXPORT Mutex
	{
	private:
	#ifdef _WIN32
		struct PlatformMutexData
		{
			Concurrency::critical_section cs;
			std::atomic<int> entryCount = 0;
		};
		
		PlatformMutexData mPlatformData;
	#else
		std::recursive_mutex mMutex;
	#endif

	public:
		Mutex(const Mutex&) = delete;
		Mutex(Mutex&&) = delete;

		Mutex();
		~Mutex();

		/**
		 * @brief
		 *  Locks the mutex to the current thread.  If locked by another thread, it will
		 *  block until the thread releases it.
		 */
		bool lock();

		/**
		 * @brief
		 *  Unlocks the mutex, waking a single other thread waiting on it.  It is an error
		 *  to call unlock if the calling thread does not hold the mutex.
		 */
		void unlock();
	};

	/**
	 * @brief
	 *  Scope based locking for a Mutex.
	 *
	 *  The constructor will take give ownership of the Mutex to
	 *  the current thread, and destructor will automatically release ownership.
	 */
	class STD_EXT_EXPORT MutexLocker
	{
	public:
		MutexLocker(const MutexLocker&) = delete;

		/**
		 * @brief
		 *  Contructs a locker that will take ownership of the passed Mutex.  This
		 *  call will block until the Mutex is available.
		 */
		MutexLocker(Mutex& M);

		MutexLocker(MutexLocker&& other) noexcept;

		/**
		 * @brief
		 *  Destructor automatically releases ownership of Mutex.
		 */
		virtual ~MutexLocker();

		MutexLocker& operator=(MutexLocker&& other);

	private:
		Mutex* mMutex;
	};
};

#endif // !_STD_EXT_CONCURRENT_MUTEX_H_