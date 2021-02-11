#include <StdExt/Concurrent/Wait.h>

#include <StdExt/Collections/Vector.h>

template<typename T, size_t size>
using Vector = StdExt::Collections::Vector<T, size>;

namespace StdExt::Concurrent
{	
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
}