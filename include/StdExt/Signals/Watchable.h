#ifndef _STD_EXT_SIGNALS_WATCHABLE_H_
#define _STD_EXT_SIGNALS_WATCHABLE_H_

#include "Event.h"

#include "../Type.h"
#include "../Exceptions.h"

#include <optional>
#include <string>

namespace StdExt::Signals
{
	using namespace StdExt;
	
	/**
	 * @brief
	 *  Concept defining allowed Watchable types.
	 */
	template<typename T>
	concept WatchableType = !ConstType<T> && ( Scaler<T> || Class<T> );

	/**
	 * @brief
	 *  This sanitizes the types used when passing update values to handling
	 *  functions.  For class types, this will always be the constant reference
	 *  form, and for scalers the actual type.
	 */
	template<typename T>
	using WatchablePassType = typename Type<T>::arg_non_copy_const_t;

	template<typename T>
	using WatchableEventBase = Event< WatchablePassType<T> >;

	template<WatchableType T>
	class Subscription;

	/**
	 * @brief
	 *  Provides an interface for values and states that can be watched.
	 *
	 * @note
	 *  For structural type, handling functions will be passed parameters by const reference.  For scaler
	 *  types, handler function parameters will be by value.  This both allows the templates to work with
	 *  pointer types, and prevents many copy constructor operations (but not all) from needing to be run.
	 */
	template<WatchableType T>
	class Watchable : public WatchableEventBase<T>
	{
		friend class Subscription<T>;

	public:
		using base_t = WatchableEventBase<T>;
		using pass_t = WatchablePassType<T>;

		/**
		 * @brief
		 *  Constructs a watchable starting with a default value of the type.
		 */
		Watchable() requires DefaultConstructable<T>
		{
		}

		/**
		 * @brief
		 *  Constructs a watchable with the passes initial value.
		 */
		Watchable(const T& init_val) requires CopyAssignable<T>
			: mLastSent(init_val)
		{
		}

		/**
		 * @brief
		 *  Constructs a watchable with the passes initial value.
		 */
		Watchable(T&& init_val) requires MoveAssignable<T>
			: mLastSent(std::move(init_val))
		{
		}

		/**
		 * @brief
		 *  Destroys the watchable object.  Any subscriptions watching it will return the
		 *  default value of T after this happens.
		 */
		virtual ~Watchable()
		{
		}

		/**
		 * @brief
		 *  Returns a reference to the value of the watchable.  Implmentations should define this
		 *  according to what is being wathched, but it the default implmentation return of
		 *  getLastSent() is typically suitable.
		 */
		virtual WatchablePassType<T> value() const
		{
			return mLastSent;
		}

	protected:

		/**
		 * @brief
		 *  Uses shouldNotify() to determine if the new value is different than what was last sent.
		 *  if so, it sets the last sent value using copy semantics and notifies subscribers of the
		 *  value change.  This should only be used if the value actually changes.
		 */
		void updateValue(const T& val) requires CopyAssignable<T>
		{
			if ( shouldNotify(mLastSent, val ) )
			{
				mLastSent = val;
				base_t::notify(mLastSent);
			}
		}
		
		/**
		 * @brief
		 *  Uses shouldNotify() to determine if the new value is different than what was last sent.
		 *  if so, it sets the last sent value using move semantics and notifies subscribers of the
		 *  value change.  This should only be used if the value actually changes.
		 */
		void updateValue(T&& val) requires Class<T> && MoveAssignable<T>
		{	
			if ( shouldNotify(mLastSent, val ) )
			{
				mLastSent = std::move(val);
				base_t::notify(mLastSent);
			}
		}

		const T& getLastSent() const
		{
			return mLastSent;
		}

		virtual bool shouldNotify(const T& last, const T& next) const
		{
			if constexpr ( FloatingPoint<T> )
			{
				return !approxEqual(last, next);
			}
			else if constexpr ( EqualityComperable<T> )
			{
				return ( !StdExt::equals(last, next) );
			}
			else
			{
				throw Exception<not_implemented>(
					"shouldNotify() has no default implmentation for type and must be overidden."
				);
			}
		}

	private:

		/**
		 * @brief
		 *  Storage of the last value sent to subscriptions.
		 */
		mutable T mLastSent;
	};
}

#endif // !_STD_EXT_SIGNALS_WATCHABLE_H_