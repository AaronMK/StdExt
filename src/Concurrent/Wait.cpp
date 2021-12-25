#include <StdExt/Concurrent/Wait.h>
#include <StdExt/Collections/Vector.h>

#ifndef _WIN32
#	include <thread>
#endif

template<typename T, size_t size>
using Vector = StdExt::Collections::Vector<T, size>;

namespace StdExt::Concurrent
{	
#ifdef _WIN32
	void WaitInternal::waitForAll(Waitable** items, size_t size)
	{
		Vector<Concurrency::event*, 32> winEvents;
		winEvents.resize(size);

		for ( uint32_t i = 0; i < size; ++i )
			winEvents[i] = items[i]->nativeWaitHandle();

		Concurrency::event::wait_for_multiple(&winEvents[0], winEvents.size(), true);
	}

	size_t WaitInternal::waitForAny(Waitable** items, size_t size)
	{
		Vector<Concurrency::event*, 32> winEvents;
		winEvents.resize(size);

		for ( uint32_t i = 0; i < size; ++i )
			winEvents[i] = items[i]->nativeWaitHandle();

		return Concurrency::event::wait_for_multiple(&winEvents[0], winEvents.size(), false);
	}
#else
	void WaitInternal::waitForAll(Waitable** items, size_t size)
	{
		for (size_t i = 0; i < size; ++i)
			items[i]->nativeWaitHandle()->wait(false);
	}

	size_t WaitInternal::waitForAny(Waitable** items, size_t size)
	{
		while ( true )
		{
			for (size_t i = 0; i < size; ++i)
			{
				if ( items[i]->nativeWaitHandle()->test() )
					return i;
			}
			std::this_thread::yield();
		}
	}
#endif
}