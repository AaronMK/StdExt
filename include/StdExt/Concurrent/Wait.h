#ifndef _STD_EXT_CONCURRENT_WAIT_H_
#define _STD_EXT_CONCURRENT_WAIT_H_

#include "../StdExt.h"
#include "../Memory.h"

#ifdef _WIN32
#	include <concrt.h>
#else
#	include <atomic>
#endif

#include <span>
#include <initializer_list>

namespace StdExt::Concurrent
{
	#ifdef _WIN32
		using WaitHandlePlatform = Concurrency::event*;
	#else
		using WaitHandlePlatform = std::atomic_flag*;
	#endif
	
	class STD_EXT_EXPORT Waitable
	{
	public:
		virtual WaitHandlePlatform nativeWaitHandle() = 0;
	};

	class STD_EXT_EXPORT WaitInternal
	{
	public:
		static void waitForAll(Waitable** items, size_t size);
		static size_t waitForAny(Waitable** items, size_t size);
	};
	
	inline void waitForAll(Waitable** items, size_t size)
	{
		WaitInternal::waitForAll(items, size);
	}

	inline size_t waitForAny(Waitable** items, size_t size)
	{
		return WaitInternal::waitForAny(items, size);
	}
	
	inline void waitForAll(const std::span<Waitable*>& items)
	{
		WaitInternal::waitForAll(items.data(), items.size());
	}

	inline size_t waitForAny(const std::span<Waitable*>& items)
	{
		return WaitInternal::waitForAny(items.data(), items.size());
	}

	inline void waitForAll(const std::initializer_list<Waitable*>& items)
	{
		Waitable** begin = access_as<Waitable**>(items.begin());
		WaitInternal::waitForAll(begin, items.size());
	}

	inline size_t waitForAny(const std::initializer_list<Waitable*>& items)
	{
		Waitable** begin = access_as<Waitable**>(items.begin());
		return WaitInternal::waitForAny(begin, items.size());
	}
}
#endif // !_STD_EXT_CONCURRENT_WAIT_H_