#ifndef _STD_EXT_SIGNALS_WATCHABLE_H_
#define _STD_EXT_SIGNALS_WATCHABLE_H_

#include "Event.h"
#include "../Type.h"

#include <optional>

namespace StdExt::Signals
{
	template<typename T>
	class Subscription;

	/**
	 * @brief
	 *  Provides an interface for values and states that are persistant and for  
	 *  which new values can be checked for a change before alerting subscribers.
	 */
	template<typename T>
	class Watchable : public Event<T>
	{
		friend class Subscription<T>;

	public:
		Watchable();
		virtual ~Watchable();

		/**
		 * @brief
		 *  Must be overridden for clients of the class, including Subscription<T> objects, to
		 *  query the current state of the watchable.
		 */
		virtual T value() const = 0;

	protected:

		/**
		 * @brief
		 *  Uses shouldNotify() to determine whether to send a notfication of a change to subscribers,
		 *  and sends the notification if it should.
		 */
		virtual void notify(const T& val);

		/**
		 * @brief
		 *  Comparison function used to determine if a notification has a different value from
		 *  the previous notification sent.  The default will use the not-equal operator when
		 *  it is supported or throws an excpetion when not supported.  For most comperable types
		 *  this should be appropriate.  Otherwise, re-implment to return true if next is different
		 *  from last and should be announced.
		 *
		 * @param last
		 *  The last value that was pushed to subscribers.
		 *
		 * @param next
		 *  The next potential value to be sent out.
		 *
		 * @return
		 *  True if next should be announced to subscribers.
		 */
		virtual bool shouldNotify(const T& last, const T& next) const;

		/**
		 * @brief
		 *  Returns true if a value has been sent.
		 */
		bool hasSent() const;

		/**
		 * @brief
		 *  Returns the last value sent, or the default of T if nothing has been sent.
		 */
		const T lastSent() const;

	private:
		std::optional<T> mLastSent;
	};

	template<typename T>
	class Subscription : protected EventHandler<T>
	{
		static_assert(Traits<T>::default_constructable, "Subscription types must be default constructable.");
		friend class Watchable<T>;

	public:
		Subscription();
		Subscription(const Watchable<T>& watchable);

		/**
		 * @brief
		 *  Attaches a Watchable to the subscription.  This will detach any
		 *  previously attached watchables.
		 */
		virtual void attach(const Watchable<T>& watchable);

		/**
		 * @brief
		 *  Detaches the subscription and calls onDetached().
		 */
		virtual void detach();

		/**
		 * @brief
		 *  Returns true if the subscription is currently tracking a Watchable<T>.
		 */
		bool isAttached() const;

		/**
		 * @brief
		 *  Return the value of the watchable to which it is subscribed, or
		 *  the default value of the type if detached.
		 */
		T value() const;

	protected:

		/**
		 * @brief
		 *  Called on an update to the watched value.  Default
		 *  implementation does nothing.
		 */
		virtual void onUpdate(const T& value);

	private:
		virtual void handleEvent(const T& value) override;
	};

	////////////////////////////////////

	template<typename T>
	Watchable<T>::Watchable()
	{
	}

	template<typename T>
	Watchable<T>::~Watchable()
	{
	}

	template<typename T>
	void Watchable<T>::notify(const T& val)
	{
		if ( !mLastSent.has_value() || shouldNotify(*mLastSent, val) )
		{
			mLastSent.emplace(val);
			Event<T>::notify(val);
		}
	}

	template<typename T>
	bool Watchable<T>::shouldNotify(const T& last, const T& next) const
	{
		if constexpr (std::is_floating_point_v<T>)
			return !approximately_equal(last, next);
		else if constexpr (Traits<T>::has_inequality)
			return (last != next);
		else
			throw invalid_operation("Test for inequality on unsupported type.");
	}

	template<typename T>
	inline bool Watchable<T>::hasSent() const
	{
		return mLastSent.has_value();
	}

	template<typename T>
	inline const T Watchable<T>::lastSent() const
	{
		return (mLastSent.has_value()) ? *mLastSent : Traits<T>::default_value();
	}

	////////////////////////////////////

	template<typename T>
	Subscription<T>::Subscription()
	{
	}

	template<typename T>
	Subscription<T>::Subscription(const Watchable<T>& watchable)
	{
		bind(watchable);
	}

	template<typename T>
	void Subscription<T>::onUpdate(const T& value)
	{
	}

	template<typename T>
	void Subscription<T>::attach(const Watchable<T>& watchable)
	{
		T nextValue = watchable.value();

		if (nextValue != value())
			onUpdate(nextValue);

		bind(watchable);
	}

	template<typename T>
	void Subscription<T>::detach()
	{
		unbind();
	}

	template<typename T>
	bool Subscription<T>::isAttached() const
	{
		return isBinded();
	}

	template<typename T>
	T Subscription<T>::value() const
	{
		if (isBinded())
			return dynamic_cast<const Watchable<T>*>(source())->value();
		else
			return Traits<T>::default_value();
	}

	template<typename T>
	void Subscription<T>::handleEvent(const T& value)
	{
		onUpdate(value);
	}
}

#endif // !_STD_EXT_SIGNALS_WATCHABLE_H_