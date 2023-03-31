#ifndef _STD_EXT_SIGNALS_SUBSCRIPTION_H_
#define _STD_EXT_SIGNALS_SUBSCRIPTION_H_

#include "Watchable.h"

#include "../Exceptions.h"

namespace StdExt::Signals
{
	template <typename T>
	using SubscriptionEventBase = EventHandler< WatchablePassType<T> >;

	template<WatchableType T>
	class Subscription : private SubscriptionEventBase<T>
	{
		using event_t = Event< WatchablePassType<T> >;
		using base_t = SubscriptionEventBase<T>;

	public:
		using pass_t = WatchablePassType<T>;

		Subscription(const Subscription&) = delete;
		Subscription& operator=(const Subscription&) = delete;

		Subscription(Subscription&&) = default;
		Subscription& operator=(Subscription&&) = default;

		/**
		 * @brief
		 *  Constructs an unbinded subscription object.
		 */
		Subscription()
		{
		}

		/**
		* @brief
		*  Constructs a subscription to the passed Watchable<T>.
		*/
		Subscription(const Watchable<T>& watchable)
		{
			attach(watchable);
		}

		/**
		 * @brief
		 *  Returns true if the subscription is attached.
		 */
		bool isAttached() const
		{
			return base_t::isBinded();
		}

		/**
		 * @brief
		 *  Attaches a Watchable to the subscription.  This will detach any
		 *  previously attached watchables. This will trigger an update event
		 *  if the value of the new Watchable is different from the old one,
		 *  or if this Subscription was previously detached.
		 */
		virtual void attach(const Watchable<T>& watchable)
		{
			bool sendUpdate = (!isAttached() || watchable.shouldNotify(value(), watchable.value()));
			event_t& evt = access_as<event_t&>(&watchable);

			base_t::bind(evt);

			if (sendUpdate)
				onUpdated(watchable.value());
		}

		/**
		 * @brief
		 *  Unsubscribes from the watchable.
		 */
		virtual void detach()
		{
			if (isAttached())
			{
				base_t::unbind();
				onDetached();
			}
		}

		Watchable<T>* sourceWatchable()
		{
			return access_as<Watchable<T>*>(base_t::sourceEvent());
		}

		const Watchable<T>* sourceWatchable() const
		{
			return access_as<const Watchable<T>*>(base_t::sourceEvent());
		}

		/**
		 * @brief
		 *  Gets the value of the Watchable<T> to which this is subscribed, or throws an invalid_operation
		 *  exception if the subscription is detached.
		 */
		WatchablePassType<T> value() const
		{
			const Watchable<T>* src = sourceWatchable();

			if (nullptr == src)
				throw invalid_operation("Can't get value for detached subscription.");
			else
				return src->value();
		}

	protected:

		/**
		 * @brief
		 *  Handler function called when the subscription is either detached because
		 *  the attached Watachable<T> object is destroyed or detach() is called.
		 *  Default implementation does nothing.
		 */
		virtual void onDetached()
		{
		}

		/**
		 * @brief
		 *  Handler function called when the attached Watchable<T> value is updated.
		 *  Default implementation does nothing.
		 */
		virtual void onUpdated(pass_t new_value)
		{
		}

	private:
		virtual void onSourceDestroyed() override final
		{
			base_t::onSourceDestroyed();
			onDetached();
		}

		virtual void handleEvent(pass_t new_value) override final
		{
			onUpdated(new_value);
		}
	};
}

#endif // !_STD_EXT_SIGNALS_SUBSCRIPTION_H_

