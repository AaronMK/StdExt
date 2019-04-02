#ifndef _STD_EXT_SIGNALS_SWAPPABLE_H_
#define _STD_EXT_SIGNALS_SWAPPABLE_H_

#include "Watchable.h"
#include "Constant.h"

#include "../Type.h"
#include "../Utility.h"

namespace StdExt
{
	template<typename T>
	class Swappable : public Watchable<T>
	{
	private:
		Subscription<T> mSubscription;

	public:
		Swappable();
		Swappable(const std::shared_ptr<Watchable<T>>& watch);

		void swapIn(const std::shared_ptr<Watchable<T>>& watch);

		virtual T value() const override;

	};

	//////////////////////////////////

	template<typename T>
	Swappable<T>::Swappable()
	{
		mSubscription.attach(
			make_dynamic_shared<Watchable<T>, ConstWatchable<T>>(Traits<T>::default_value())
		);
	}

	template<typename T>
	Swappable<T>::Swappable(const std::shared_ptr<Watchable<T>>& watch)
	{
		mSubscription.attach(watch);
	}

	template<typename T>
	void Swappable<T>::swapIn(const std::shared_ptr<Watchable<T>>& watch)
	{
		bool changed = true;

		if constexpr (Traits<T>::has_inequality)
			changed = (watch->value() != mSubscription->value());

		mSubscription.attach(watch)
	}

	template<typename T>
	T Swappable<T>::value() const
	{
		return mSubscription.value();
	}
}

#endif // !_STD_EXT_SIGNALS_SWAPPABLE_H_