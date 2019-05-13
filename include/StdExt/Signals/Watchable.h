#ifndef _STD_EXT_SIGNALS_WATCHABLE_H_
#define _STD_EXT_SIGNALS_WATCHABLE_H_

#include "Event.h"

namespace StdExt::Signals
{
	template<typename T>
	class Subscription;

	template<typename T>
	class Watchable
	{
		friend class Subscription<T>;

	private:
		Event<T> mEvent;

	protected:

		/**
		* @brief
		*  Announces an update to all subscribers.
		*/
		void announceUpdate(const T& val);

	public:
		virtual T value() const = 0;
	};

	template<typename T>
	using WatchRef = std::shared_ptr<Watchable<T>>;

	template<typename T>
	class Subscription : private EventHandler<T>
	{
		WatchRef<T> mWatchable;
	public:
		Subscription();
		Subscription(const WatchRef<T>& watchable);

		virtual void onUpdate(const T& value);

		/**
		 * @brief
		 *  Attaches a watchable to the subscription.  This will detach any
		 *  previously attached watchables.
		 */
		void attach(const WatchRef<T>& watchable);
		void detach();

		bool isAttached() const;

		T value() const;

	private:
		virtual void handleEvent(const T& value) override;
	};

	////////////////////////////////////

	template<typename T>
	void Watchable<T>::announceUpdate(const T& val)
	{
		mEvent.invoke(val);
	}

	////////////////////////////////////

	template<typename T>
	Subscription<T>::Subscription()
	{
	}

	template<typename T>
	Subscription<T>::Subscription(const WatchRef<T>& watchable)
		: mWatchable(watchable)
	{
		bind(mWatchable->mEvent);
	}

	template<typename T>
	void Subscription<T>::onUpdate(const T& value)
	{
	}

	template<typename T>
	void Subscription<T>::attach(const WatchRef<T>& watchable)
	{
		if (mWatchable)
			unbind();

		mWatchable = watchable;
		bind(mWatchable->mEvent);
	}

	template<typename T>
	void Subscription<T>::detach()
	{
		unbind();
		mWatchable.reset();
	}

	template<typename T>
	bool Subscription<T>::isAttached() const
	{
		return isBinded();
	}

	template<typename T>
	T Subscription<T>::value() const
	{
		if (!mWatchable)
			throw invalid_operation("Attemting to get the value of an unattached subscription.");

		return mWatchable->value();
	}

	template<typename T>
	void Subscription<T>::handleEvent(const T& value)
	{
		onUpdate(value);
	}
}

#endif // !_STD_EXT_SIGNALS_WATCHABLE_H_