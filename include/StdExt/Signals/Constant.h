#ifndef _STD_EXT_SIGNALS_CONSTANT_H_
#define _STD_EXT_SIGNALS_CONSTANT_H_

#include "Watchable.h"

namespace StdExt
{
	template<typename T>
	class ConstWatchable : public Watchable<T>
	{
	private:
		T mValue;

	public:
		ConstWatchable(const T& value);
		ConstWatchable(T&& value);

		virtual T value() const override;
	};

	////////////////////////////////////

	template<typename T>
	ConstWatchable<T>::ConstWatchable(const T& value)
		: mValue(value)
	{
	}

	template<typename T>
	ConstWatchable<T>::ConstWatchable(T&& value)
		: mValue(std::move(value))
	{
	}

	template<typename T>
	T ConstWatchable<T>::value() const
	{
		return mValue;
	}
}

#endif // !_STD_EXT_SIGNALS_CONSTANT_H_