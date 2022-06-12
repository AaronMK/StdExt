#ifndef _STD_EXT_SIGNALS_CONSTANT_H_
#define _STD_EXT_SIGNALS_CONSTANT_H_

#include "Watchable.h"

namespace StdExt::Signals
{
	template<typename T>
	class ConstWatchable : public Watchable<T>
	{
		using base_t = Watchable<T>;

	public:
		ConstWatchable(const T& value)
		{
			base_t::updateValue(value);
		}

		ConstWatchable(T&& value)
		{
			base_t::updateValue(std::move(value));
		}
	};
}

#endif // !_STD_EXT_SIGNALS_CONSTANT_H_