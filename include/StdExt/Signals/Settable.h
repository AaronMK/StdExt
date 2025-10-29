#ifndef _STD_EXT_SIGNALS_SETTABLE_H_
#define _STD_EXT_SIGNALS_SETTABLE_H_

#include "Watchable.h"

#include "../Utility.h"

namespace StdExt::Signals
{
	template<WatchableType T>
	class Settable : public Watchable<T>
	{
	public:
		using base_t = Watchable<T>;

		Settable() requires DefaultConstructable<T>
			: Watchable<T>(T{})
		{
		}

		Settable(const T& initValue) requires CopyConstructable<T>
			: Watchable<T>(initValue)
		{

		}

		Settable(T&& initValue) requires Class<T> && MoveConstructable<T>
			: Watchable<T>(std::move(initValue))
		{
		}

		void setValue(const T& val) requires CopyAssignable<T>
		{
			base_t::updateValue(val);
		}

		void setValue(T&& val) requires Class<T> && MoveAssignable<T>
		{
			base_t::updateValue(std::move(val));
		}
	};
}

#endif // !_STD_EXT_SIGNALS_SETTABLE_H_