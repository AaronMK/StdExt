#ifndef _STD_EXT_SIGNALS_CALLABLE_HANDLER_H_
#define _STD_EXT_SIGNALS_CALLABLE_HANDLER_H_

#include "Subscription.h"

namespace StdExt::Signals
{
	/**
	 * @brief
	 *  An event handler that stores any compatible callable type,
	 *  and responds to the event by calling the stored hendler.
	 * 
	 * @tparam handler_t
	 *  The type of the event handler.  It must be callable with args_t
	 *  arguments and a void return.
	 * 
	 * @tparam args_t
	 *  The type parameters of the event.  This should match the type
	 *  parameters of the Event to which this handler will bind.
	 * 
	 * @see makeEventHandler();
	 */
	template<typename handler_t, typename ...args_t>
		requires CallableWith<handler_t, void, args_t...>
	class CallableEventHandler : public EventHandler<args_t...>
	{
	private:
		handler_t mHandler;

		using base_t = EventHandler<args_t...>;

	public:
		CallableEventHandler(const CallableEventHandler&) = delete;
		CallableEventHandler& operator=(const CallableEventHandler&) = delete;

		CallableEventHandler(CallableEventHandler&&)
			requires MoveConstructable<handler_t> = default;

		CallableEventHandler& operator=(CallableEventHandler&&)
			requires MoveAssignable<handler_t> = default;
		
		CallableEventHandler()
			requires Defaultable<handler_t>
			: mHandler{}
		{
		}

		CallableEventHandler(const handler_t& handler)
			requires CopyConstructable<handler_t>
			: mHandler(handler)
		{
		}

		CallableEventHandler(handler_t&& handler)
			requires MoveConstructable<handler_t>
			: mHandler(std::move(handler))
		{
		}

		CallableEventHandler(const Event<args_t...>& evt, const handler_t& handler)
			requires CopyConstructable<handler_t>
			: mHandler(handler)
		{
			bind(evt);
		}

		CallableEventHandler(const Event<args_t...>& evt, handler_t&& handler)
			requires MoveConstructable<handler_t>
			: mHandler(std::move(handler))
		{
			bind(evt);
		}

		void handleEvent(args_t ...args) override
		{
			if constexpr ( ImplicitlyConvertableTo<bool, handler_t> )
			{
				if ( mHandler )
					mHandler(std::forward<args_t...>(args)...);
			}
			else
			{
				mHandler(std::forward<args_t...>(args)...);
			}
		}

		/**
		 * @brief
		 *  Sets the handler object by move construction.
		 * 
		 * @throws invalid_operation
		 *  When attempting to set a handler while this EventHandler is binded.
		 */
		void setHandler(handler_t&& handler)
			requires MoveAssignable<handler_t>
		{
			if ( base_t::isBinded() )
				throw Exception<invalid_operation>("Can't set the function of a binded event handler.");

			mHandler = std::move(handler);
		}

		/**
		 * @brief
		 *  Sets the handler object by copy construction.
		 *
		 * @throws invalid_operation
		 *  When attempting to set a handler while this EventHandler is binded.
		 */
		void setHandler(const handler_t& handler)
			requires CopyAssignable<handler_t>
		{
			if ( base_t::isBinded() )
				throw Exception<invalid_operation>("Can't set the function of a binded event handler.");

			mHandler = handler;
		}

		/**
		 * @brief
		 *  Clears the event handler.
		 *
		 * @throws invalid_operation
		 *  When attempting to clear the handler while this EventHandler is binded.
		 */
		void clearHandler()
			requires Defaultable<handler_t>
		{
			if ( base_t::isBinded() )
				throw Exception<invalid_operation>("Can't remove the function of a binded event handler.");

			mHandler = handler_t{};
		}
	};

	///@{
	/**
	 * @brief
	 *  Creates CallableEventHandler with correct template parameters for the handler type
	 *  passed.  This is useful for creating handlers for lambda functions, but can be used
	 *  with any compatible handler type.
	 *
	 * @note
	 *  args_t must be manually specified as a template parameter.
	 *
	 * @param handler
	 *  The handler object that will respond to events.
	 *
	 * @param evt
	 *  A convenience parameter to automatically attach the event handler
	 *  to a Event.
	 *
	 * @tparam handler_t
	 *  The type of the event handler.  It must be callable with a
	 *  args_t arguments and a void return.
	 *
	 * @tparam args_t
	 *  The type parameters of the event.  This should match the type
	 *  parameters of the Event to which this handler will bind.
	 */
	template<typename ...args_t, CallableWith<void, args_t...> handler_t>
	auto makeEventHandler(const handler_t& handler)
	{
		using handle_arg_type = decltype(handler);
		using handle_strip_type = Type<handle_arg_type>::stripped_t;

		return CallableEventHandler<handle_strip_type, args_t...>(std::forward<handle_arg_type>(handler) );
	}

	template<typename ...args_t, CallableWith<void, args_t...> handler_t>
	auto makeEventHandler(handler_t&& handler)
	{
		using handle_arg_type = decltype(handler);
		using handle_strip_type = Type<handle_arg_type>::stripped_t;

		return CallableEventHandler<handle_strip_type, args_t...>(std::forward<handle_arg_type>(handler));
	}

	template<typename ...args_t, CallableWith<void, args_t...> handler_t>
	auto makeEventHandler(const handler_t& handler, const Event<args_t...>& evt)
	{
		auto ret = makeEventHandler<args_t...>(handler);
		ret.bind(evt);

		return ret;
	}

	template<typename ...args_t, CallableWith<void, args_t...> handler_t>
	auto makeEventHandler(handler_t&& handler, const Event<args_t...>& evt)
	{
		using handle_arg_type = decltype(handler);

		auto ret = makeEventHandler<args_t...>(std::forward<handle_arg_type>(handler));
		ret.bind(evt);

		return ret;
	}
	///@}

	/**
	 * @brief
	 *  An update handler that stores any compatible callable type,
	 *  and responds to the event by calling the stored hendler.
	 *
	 * @tparam handler_t
	 *  The type of the update handler.  It must be callable with a
	 *  WatchablePassType<value_t> arguments and a void return.
	 *
	 * @tparam value_t
	 *  The type parameter of the update.  This should match the type
	 *  parameters of the Watchable to which this handler will attach.
	 *
	 * @see makeUpdateHandler();
	 */
	template<typename handler_t, typename value_t>
		requires CallableWith<handler_t, void, WatchablePassType<value_t> >
	class CallableUpdateHandler : public Subscription<value_t>
	{
	private:
		handler_t mHandler;

		using base_t = Subscription<value_t>;

	public:
		CallableUpdateHandler(const CallableUpdateHandler&) = delete;
		CallableUpdateHandler& operator=(const CallableUpdateHandler&) = delete;

		CallableUpdateHandler(CallableUpdateHandler&&)
			requires MoveConstructable<handler_t> = default;

		CallableUpdateHandler& operator=(CallableUpdateHandler&&)
			requires MoveAssignable<handler_t> = default;

		CallableUpdateHandler()
			requires Defaultable<handler_t>
		: mHandler{}
		{
		}

		CallableUpdateHandler(const handler_t& handler)
			requires CopyConstructable<handler_t>
			: mHandler(handler)
		{
		}

		CallableUpdateHandler(handler_t&& handler)
			requires MoveConstructable<handler_t>
			: mHandler(std::move(handler))
		{
		}

		CallableUpdateHandler(const Watchable<value_t>& watched, const handler_t& handler)
			requires CopyConstructable<handler_t>
			: mHandler(handler)
		{
			attach(watched);
		}

		CallableUpdateHandler(const Watchable<value_t>& watched, handler_t&& handler)
			requires MoveConstructable<handler_t>
			: mHandler(std::move(handler))
		{
			attach(watched);
		}

		void onUpdated(base_t::pass_t val) override
		{
			auto ptr = &CallableUpdateHandler::detach;

			if constexpr (ImplicitlyConvertableTo<bool, handler_t>)
			{
				if ( mHandler )
					mHandler(val);
			}
			else
			{
				mHandler(val);
			}
		}

		void setHandler(handler_t&& handler)
			requires MoveAssignable<handler_t>
		{
			if (base_t::isAttached())
				throw Exception<invalid_operation>("Can't set the function of an attached subscription");

			mHandler = std::move(handler);
		}

		void setHandler(const handler_t& handler)
			requires CopyAssignable<handler_t>
		{
			if (base_t::isAttached())
				throw Exception<invalid_operation>("Can't set the function of an attached subscription");

			mHandler = handler;
		}

		void clearHandler()
			requires Defaultable<handler_t>
		{
			if (base_t::isAttached())
				throw Exception<invalid_operation>("Can't set the function of an attached subscription");

			mHandler = handler_t{};
		}
	};

	///@{
	/**
	 * @brief
	 *  Creates CallableUpdateHandler with correct template parameters for the handler type
	 *  passed.  This is useful for creating handlers for lambda functions, but can be used
	 *  with any compatible handler type.
	 * 
	 * @note
	 *  watch_t must be manually specified as a template parameter.
	 *
	 * @param handler
	 *  The handler object that will respond to updates.
	 *
	 * @param watched
	 *  A convenience parameter to automatically attach the update handler
	 *  to a Watchable.
	 * 
	 * @tparam handler_t
	 *  The type of the update handler.  It must be callable with a
	 *  WatchablePassType<watch_t> arguments and a void return.
	 *
	 * @tparam watch_t
	 *  The type parameter of the update.  This should match the type
	 *  parameters of the Watchable to which this handler will attach.
	 */
	template<typename watch_t, CallableWith< void, WatchablePassType<watch_t> > handler_t>
	auto makeUpdateHandler(const handler_t& handler)
	{
		using handle_arg_type = decltype(handler);
		using handle_strip_type = Type<handle_arg_type>::stripped_t;

		return CallableUpdateHandler<handle_strip_type, watch_t>(std::forward<handle_arg_type>(handler));
	}

	template<typename watch_t, CallableWith< void, WatchablePassType<watch_t> > handler_t>
	auto makeUpdateHandler(handler_t&& handler)
	{
		using handle_arg_type = decltype(handler);
		using handle_strip_type = Type<handle_arg_type>::stripped_t;

		return CallableUpdateHandler<handle_strip_type, watch_t>(std::forward<handle_arg_type>(handler));
	}

	template<typename watch_t, CallableWith< void, WatchablePassType<watch_t> > handler_t>
	auto makeUpdateHandler(const handler_t& handler, const Watchable<watch_t>& watched)
	{
		auto ret = makeUpdateHandler<watch_t>(handler);
		ret.attach(watched);

		return ret;
	}

	template<typename watch_t, CallableWith< void, WatchablePassType<watch_t> > handler_t>
	auto makeUpdateHandler(handler_t&& handler, const Watchable<watch_t>& watched)
	{
		using handle_arg_type = decltype(handler);

		auto ret = makeUpdateHandler<watch_t>(std::forward<handle_arg_type>(handler));
		ret.attach(watched);

		return ret;
	}
	///@}
}

#endif // !_STD_EXT_SIGNALS_CALLABLE_HANDLER_H_