#ifndef _STD_EXT_CONCURRENT_UTILITY_H_
#define _STD_EXT_CONCURRENT_UTILITY_H_

#include "../Callable.h"

#include <semaphore>

namespace StdExt::Concurrent
{
	class SemLock
	{
	private:
		CallablePtr<void(std::ptrdiff_t)> mUnlockFunc;

	public:
		constexpr SemLock() noexcept = default;
		SemLock(const SemLock&) = delete;

		SemLock(SemLock&& other) noexcept
			: mUnlockFunc( std::move(other.mUnlockFunc) )
		{
		}

		template<std::ptrdiff_t least_max_value>
		SemLock(std::counting_semaphore<least_max_value>& sem) noexcept
		{
			sem.acquire();
			mUnlockFunc.bind<&std::counting_semaphore<least_max_value>::release>(&sem);
		}

		template<std::ptrdiff_t least_max_value, typename Rep, typename Period>
		SemLock(std::counting_semaphore<least_max_value>& sem, std::chrono::duration<Rep, Period> timeout) noexcept
		{
			if ( sem.try_acquire_for(timeout) )
				mUnlockFunc.bind<&std::counting_semaphore<least_max_value>::release>(&sem);
		}

		~SemLock() noexcept
		{
			if (mUnlockFunc)
				mUnlockFunc(1);
		}

		template<std::ptrdiff_t least_max_value>
		void acquire(std::counting_semaphore<least_max_value>& sem)
		{
			CallablePtr<void(std::ptrdiff_t)> next_release = bind<&std::counting_semaphore<least_max_value>::release>(&sem);

			if ( next_release == mUnlockFunc )
				return;

			if (mUnlockFunc)
				mUnlockFunc(1);
			
			sem.acquire();
			mUnlockFunc = next_release;
		}
		
		template<std::ptrdiff_t least_max_value, typename Rep, typename Period>
		bool tryAquire(std::counting_semaphore<least_max_value>& sem, std::chrono::duration<Rep, Period> timeout)
		{
			CallablePtr<void(std::ptrdiff_t)> next_release = bind<&std::counting_semaphore<least_max_value>::release>(&sem);

			if ( next_release == mUnlockFunc )
				return true;

			if ( sem.try_acquire_for(timeout) )
			{
				if (mUnlockFunc)
					mUnlockFunc(1);

				mUnlockFunc = next_release;

				return true;
			}

			return false;
		}

		void unlock()
		{
			if (mUnlockFunc)
			{
				mUnlockFunc(1);
				mUnlockFunc.clear();
			}
		}

		SemLock& operator=(const SemLock&) = delete;
		SemLock& operator=(SemLock&& other)
		{
			if ( this == &other )
				return *this;

			if (mUnlockFunc)
				mUnlockFunc(1);

			mUnlockFunc = std::move(other.mUnlockFunc);
			return *this;
		}

		operator bool() const noexcept
		{
			return ( mUnlockFunc );
		}

		bool owns_lock() const noexcept
		{
			return ( mUnlockFunc );
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_UTILITY_H_
