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
		virtual void setValue(const T& val) = 0;
		virtual T value() const = 0;
	};

	template<typename T>
	class LocalSettable : public Settable<T>
	{
		T mValue;

	public:
		LocalSettable();
		LocalSettable(const T& initValue);

		virtual void setValue(const T& val) override;
		virtual T value() const override;
	};


	template<typename T>
	using SetRef = std::shared_ptr<Settable<T>>;

	
	////////////////////////////////////
	
	template<typename T>
	LocalSettable<T>::LocalSettable()
	{
	}

	template<typename T>
	LocalSettable<T>::LocalSettable(const T& initValue)
		: mValue(initValue)
	{
	}

	template<typename T>
	void LocalSettable<T>::setValue(const T& val)
	{
		if constexpr ( Traits<T>::has_inequality )
		{
			if ( update(mValue, val) )
				announceUpdate(mValue);
		}
		else
		{
			mValue = val;
			announceUpdate(mValue);
		}
	}

	template<typename T>
	T LocalSettable<T>::value() const
	{
		return mValue;
	}
}

#endif // !_STD_EXT_SIGNALS_SETTABLE_H_