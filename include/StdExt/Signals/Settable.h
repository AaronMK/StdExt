#ifndef _STD_EXT_SIGNALS_SETTABLE_H_
#define _STD_EXT_SIGNALS_SETTABLE_H_

#include "Watchable.h"

#include "../Utility.h"

namespace StdExt::Signals
{
	template<typename T>
	class Settable : public Watchable<T>
	{
	public:
		Settable();
		Settable(const T& initValue);

		virtual T value() const override;
		virtual void setValue(const T& val);
	};
	
	////////////////////////////////////
	
	template<typename T>
	Settable<T>::Settable()
	{
		setValue(Traits<T>::default_value());
	}

	template<typename T>
	Settable<T>::Settable(const T& initValue)
	{
		setValue(initValue);
	}

	template<typename T>
	void Settable<T>::setValue(const T& val)
	{
		notify(val);
	}

	template<typename T>
	T Settable<T>::value() const
	{
		return lastSent();
	}
}

#endif // !_STD_EXT_SIGNALS_SETTABLE_H_