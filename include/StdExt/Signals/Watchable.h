#ifndef _STD_EXT_SIGNALS_WATCHABLE_H_
#define _STD_EXT_SIGNALS_WATCHABLE_H_

#include "Event.h"
#include "../Type.h"

#include <optional>

namespace StdExt::Signals
{
	using namespace StdExt;

	template<typename T>
	class Subscription;

	template<typename T>
	using trivial_t = typename Traits<T>::arg_non_copy_const_t;

	/**
	 * @brief
	 *  Provides an interface for values and states that can be watched.
	 *
	 * @note
	 *  For structural type, handling functions will be passed parameters by const reference.  For scaler
	 *  types, handler function parameters will be by value.  This both allows the templates to work with
	 *  pointer types, and prevents many copy constructor operations (but not all) from needing to be run.
	 */
	template<HasNotEqual T>
	class Watchable : public Event<trivial_t<T>>
	{
		static_assert(
			Traits<T>::is_value || (Traits<T>::is_stripped && Traits<T>::copy_assignable && Traits<T>::default_constructable),
			"T must be a stripped class/struct or a value type."
		);

		friend class Subscription<T>;

	public:
		using pass_t = typename trivial_t<T>;

		Watchable() requires Defaultable<T>;
		Watchable(const T& init_val);
		Watchable(T&& init_val);

		virtual ~Watchable();

		/**
		 * @brief
		 *  Returns the result calcValue(), if Quick Value is enabled, it will return
		 *  the last value passed to notify().
		 */
		T value() const;

		/**
		 * @brief
		 *  If true, calls to value() will just retireve the last value sent by this
		 *  watchable, instead of having the watchable recheck dependent states. This
		 *  defaults to false.
		 */
		void setQuickValue(bool setting);

	protected:

		/**
		 * @brief
		 *  Must be overridden for clients of the class, including Subscription<T> objects, to
		 *  query the current state of the watchable.
		 */
		virtual T calcValue() const = 0;

		/**
		 * @brief
		 *  Uses shouldNotify() to determine whether to send a notfication of a change to subscribers,
		 *  and sends the notification if it should.
		 */
		virtual void notify(pass_t val);

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
		virtual bool shouldNotify(pass_t last, pass_t next) const;

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
		static constexpr uint32_t VALUE_SENT = 0x0001;
		static constexpr uint32_t VALUE_GETS_LAST_SENT = 0x0002;

		mutable T mLastSent;
		mutable uint32_t mFlags = 0;
	};

	template<typename T>
	class Subscription : private EventHandler<trivial_t<T>>
	{
		static_assert(Traits<T>::default_constructable, "Subscription types must be default constructable.");
		friend class Watchable<T>;

	public:
		using pass_t = typename trivial_t<T>;

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

		void blockUpdates(bool _block);
		bool updatesBlocked() const;

	protected:

		/**
		 * @brief
		 *  Called on an update to the watched value.  Default
		 *  implementation does nothing.
		 */
		virtual void onUpdate(pass_t value);

	private:
		virtual void handleEvent(pass_t value) override;
	};

	////////////////////////////////////

	template<HasNotEqual T>
	Watchable<T>::Watchable() requires Defaultable<T>
		: mLastSent(Traits<T>::default_value())
	{
		mFlags = 0;
	}
	
	template<HasNotEqual T>
	Watchable<T>::Watchable(const T& init_val)
		: mLastSent(init_val)
	{
		mFlags = 0;
	}

	template<HasNotEqual T>
	Watchable<T>::Watchable(T&& init_val)
		: mLastSent(std::move(init_val))
	{
		mFlags = 0;
	}

	template<HasNotEqual T>
	Watchable<T>::~Watchable()
	{
	}
	
	template<HasNotEqual T>
	T Watchable<T>::value() const
	{
		bool send_last = (0 != (mFlags & VALUE_GETS_LAST_SENT));
		bool has_been_sent = (0 != (mFlags & VALUE_SENT));

		if (!send_last || !has_been_sent)
			mLastSent = calcValue();

		mFlags |= VALUE_SENT;
		return mLastSent;
	}

	template<HasNotEqual T>
	void Watchable<T>::setQuickValue(bool setting)
	{
		if (setting)
			mFlags |= VALUE_GETS_LAST_SENT;
		else
			mFlags &= ~VALUE_GETS_LAST_SENT;
	}

	template<HasNotEqual T>
	T Watchable<T>::calcValue() const
	{
		return mLastSent;
	}

	template<HasNotEqual T>
	void Watchable<T>::notify(pass_t val)
	{
		if ( !hasSent() || shouldNotify(mLastSent, val) )
		{
			mLastSent = val;
			mFlags |= VALUE_SENT;
			Event<trivial_t<T>>::notify(val);
		}
	}

	template<HasNotEqual T>
	bool Watchable<T>::shouldNotify(pass_t last, pass_t next) const
	{
		if constexpr ( FloatingPoint<T> )
			return !approximatelyEqual(last, next);
		else
			return (last != next);
	}

	template<HasNotEqual T>
	bool Watchable<T>::hasSent() const
	{
		return (0 != (mFlags & VALUE_SENT));
	}

	template<HasNotEqual T>
	const T Watchable<T>::lastSent() const
	{
		return (hasSent()) ? mLastSent : Traits<T>::default_value();
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
	void Subscription<T>::onUpdate(pass_t value)
	{
	}

	template<typename T>
	void Subscription<T>::attach(const Watchable<T>& watchable)
	{
		if (blocked())
		{
			bind(watchable);
		}
		else if (isBinded())
		{
			T prevValue = value();
			bind(watchable);
			T nextValue = watchable.value();

			if (prevValue != nextValue)
				onUpdate(nextValue);
		}
		else
		{
			bind(watchable);
			onUpdate(value());
		}
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
			return cast_pointer<const Watchable<T>*>(source())->value();
		else
			return Traits<T>::default_value();
	}

	template<typename T>
	void Subscription<T>::blockUpdates(bool _block)
	{
		block(_block);
	}

	template<typename T>
	bool Subscription<T>::updatesBlocked() const
	{
		return blocked();
	}

	template<typename T>
	void Subscription<T>::handleEvent(pass_t value)
	{
		if (!blocked())
			onUpdate(value);
	}
}

#endif // !_STD_EXT_SIGNALS_WATCHABLE_H_