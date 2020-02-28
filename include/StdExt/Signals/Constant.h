#ifndef _STD_EXT_SIGNALS_CONSTANT_H_
#define _STD_EXT_SIGNALS_CONSTANT_H_

#include "Watchable.h"

namespace StdExt::Signals
{
	template<typename T>
	class ConstWatchable : public Watchable<T>
	{
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
		setQuickValue(true);
		Watchable<T>::notify(value);
	}

	template<typename T>
	ConstWatchable<T>::ConstWatchable(T&& value)
	{
		setQuickValue(true);
		Watchable<T>::notify(value);
	}

	template<typename T>
	T ConstWatchable<T>::calcValue() const
	{
		return lastSent();
	}
}

#endif // !_STD_EXT_SIGNALS_CONSTANT_H_