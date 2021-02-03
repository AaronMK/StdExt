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
		ConstWatchable(const T& value);
		ConstWatchable(T&& value);

	protected:
		virtual T calcValue() const override;
	};

	////////////////////////////////////

	template<typename T>
	ConstWatchable<T>::ConstWatchable(const T& value)
	{
		base_t::setQuickValue(true);
		base_t::notify(value);
	}

	template<typename T>
	ConstWatchable<T>::ConstWatchable(T&& value)
	{
		base_t::setQuickValue(true);
		base_t::notify(value);
	}

	template<typename T>
	T ConstWatchable<T>::calcValue() const
	{
		return base_t::lastSent();
	}
}

#endif // !_STD_EXT_SIGNALS_CONSTANT_H_