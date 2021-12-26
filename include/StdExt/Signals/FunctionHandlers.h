#ifndef _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_
#define _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_

#include "Event.h"
#include "Watchable.h"
#include "Subscription.h"

#include "../Exceptions.h"
#include "../FunctionPtr.h"

#include <functional>

namespace StdExt::Signals
{
	template<typename  ...args_t>
	class FunctionEventHandler : public EventHandler<args_t...>
	{
	public:
		using func_t = std::function<void(args_t...)>;
		using event_t = Event<args_t...>;

		using base_t = EventHandler<args_t...>;

		FunctionEventHandler()
		{
		}

		FunctionEventHandler(func_t&& func)
		{
			mFunc = std::move(func);
		}

		FunctionEventHandler(const event_t& evt, func_t&& func)
		{
			mFunc = std::move(func);
			base_t::bind(evt);
		}
		
		FunctionEventHandler(const func_t& func)
		{
			mFunc = func;
		}

		FunctionEventHandler(const event_t& evt, const func_t& func)
		{
			mFunc = func;
			base_t::bind(evt);
		}

		void setFunction(func_t&& func)
		{
			if (base_t::isBinded() )
				throw Exception<invalid_operation>("Can't set the function of a binded event handler.");

			mFunc = std::move(func);
		}

		void setFunction(const func_t& func)
		{
			if (base_t::isBinded() )
				throw Exception<invalid_operation>("Can't set the function of a binded event handler.");

			mFunc = func;
		}

		void clearFunction()
		{
			if (base_t::isBinded() )
				throw Exception<invalid_operation>("Can't remove the function of a binded event handler.");

			mFunc = func_t();
		}

	protected:
		virtual void handleEvent(args_t ...args)
		{
			if ( mFunc )
				mFunc(std::forward<args_t>(args)...);
		}

	private:
		func_t mFunc;
	};

	template<typename  ...args_t>
	class FunctionPtrEventHandler : public EventHandler<args_t...>
	{
	public:
		using func_t = FunctionPtr<void, args_t...>;
		using event_t = Event<args_t...>;

		using base_t = EventHandler<args_t...>;

		FunctionPtrEventHandler()
		{
		}
		
		FunctionPtrEventHandler(const func_t& func)
		{
			mFunc = func;
		}

		FunctionPtrEventHandler(const event_t& evt, const func_t& func)
		{
			mFunc = func;
			bind(evt);
		}

		void setFunction(const func_t& func)
		{
			if ( base_t::isBinded() )
				throw Exception<invalid_operation>("Can't set the function on a handler that is already binded.");

			mFunc = func;
		}

		void clearFunction()
		{
			if ( base_t::isBinded() )
				throw Exception<invalid_operation>("Can't set the function on a handler that is already binded.");

			mFunc.clear();
		}
	protected:
		virtual void handleEvent(args_t ...args)
		{
			if ( mFunc )
				mFunc(std::forward<args_t>(args)...);
		}

	private:
		func_t mFunc;
	};

	template<typename  T>
	class FunctionUpdateHandler : public Subscription<T>
	{
	public:
		using func_t = std::function<void(WatchablePassType<T>)>;
		using pass_t = WatchablePassType<T>;

		using base_t = Subscription<T>;

		FunctionUpdateHandler()
		{
		}

		FunctionUpdateHandler(func_t&& func)
		{
			mFunc = std::move(func);
		}

		FunctionUpdateHandler(const Watchable<T>& sub, func_t&& func)
		{
			mFunc = std::move(func);
			base_t::attach(sub);
		}

		FunctionUpdateHandler(const func_t& func)
		{
			mFunc = func;
		}

		FunctionUpdateHandler(const Watchable<T>& sub, const func_t& func)
		{
			mFunc = func;
			base_t::attach(sub);
		}

		void setFunction(func_t&& func)
		{
			if ( base_t::isAttached() )
				throw Exception<invalid_operation>("Can't set the function on a handler that is already attached.");

			mFunc = std::move(func);
		}

		void setFunction(const func_t& func)
		{
			if ( base_t::isAttached() )
				throw Exception<invalid_operation>("Can't set the function on a handler that is already attached.");

			mFunc = func;
		}

		void clearFunction()
		{
			if ( base_t::isAttached() )
				throw Exception<invalid_operation>("Can't clear the function on a handler that is already binded.");

			mFunc = func_t();
		}

	protected:
		virtual void onUpdated(pass_t value) override
		{
			if (mFunc)
				mFunc(value); 
		}

	private:
		func_t mFunc;
	};

	template<typename  T>
	class FunctionPtrUpdateHandler : public Subscription<T>
	{
	public:
		using base_t = Subscription<T>;

		using pass_t = WatchablePassType<T>;
		using func_t = FunctionPtr<void, pass_t>;

		FunctionPtrUpdateHandler()
		{
		}

		FunctionPtrUpdateHandler(const func_t& func)
		{
			mFunc = func;
		}

		FunctionPtrUpdateHandler(const Watchable<T>& sub, const func_t& func)
		{
			mFunc = func;
			base_t::attach(sub);
		}

		void setFunction(const func_t& func)
		{
			if ( base_t::isAttached() )
				throw Exception<invalid_operation>("Can't set function on a handler that is already binded.");

			mFunc = std::move(func);
		}

		void clearFunction()
		{
			if ( base_t::isAttached() )
				throw Exception<invalid_operation>("Can't clear the function on a handler that is already binded.");

			mFunc.clear();
		}

		virtual void onUpdate(pass_t value) override
		{
			if ( mFunc )
				mFunc(value);
		}

	private:
		func_t mFunc;
	};
}

#endif // !_STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_