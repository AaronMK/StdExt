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

		virtual void setValue(const T& val);

	protected:
		virtual T calcValue() const override;
	};
	
	////////////////////////////////////
	
	template<typename T>
	Settable<T>::Settable()
	{
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
	T Settable<T>::calcValue() const
	{
		return lastSent();
	}
}

#endif // !_STD_EXT_SIGNALS_SETTABLE_H_