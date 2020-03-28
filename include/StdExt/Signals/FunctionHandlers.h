#ifndef _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_
#define _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_

#include "Event.h"
#include "Watchable.h"
#include "../FunctionPtr.h"

#include <functional>

namespace StdExt::Signals
{
	template<typename  ...args_t>
	class FunctionEventHandler : public EventHandler<args_t...>
	{
	public:
		using func_t = std::function<void(args_t...)>;

		FunctionEventHandler();

		FunctionEventHandler(func_t&& func);
		FunctionEventHandler(const Event< args_t...>& evt, func_t&& func);
		
		FunctionEventHandler(const func_t& func);
		FunctionEventHandler(const Event<args_t...>& evt, const func_t& func);

		void setFunction(func_t&& func);
		void setFunction(const func_t& func);

		void clearFunction();

	protected:
		virtual void handleEvent(args_t ...args);

	private:
		func_t mFunc;
	};

	template<typename  ...args_t>
	class FunctionPtrEventHandler : public EventHandler<args_t...>
	{
	public:
		using func_t = FunctionPtr<void, args_t...>;

		FunctionPtrEventHandler();
		
		FunctionPtrEventHandler(const func_t& func);
		FunctionPtrEventHandler(const Event<args_t...>& evt, const func_t& func);

		void setFunction(func_t&& func);
		void setFunction(const func_t& func);

		void clearFunction();

	protected:
		virtual void handleEvent(args_t ...args);

	private:
		func_t mFunc;
	};

	template<typename  T>
	class FunctionUpdateHandler : public Subscription<T>
	{
	public:
		using pass_t = typename Subscription<T>::pass_t;
		using func_t = std::function<void(pass_t)>;

		FunctionUpdateHandler();

		FunctionUpdateHandler(func_t&& func);
		FunctionUpdateHandler(const Watchable<T>& sub, func_t&& func);

		FunctionUpdateHandler(const func_t& func);
		FunctionUpdateHandler(const Watchable<T>& sub, const func_t& func);

		void setFunction(func_t&& func);
		void setFunction(const func_t& func);

		void clearFunction();

		virtual void onUpdate(pass_t value) override;

	private:
		func_t mFunc;
	};

	template<typename  T>
	class FunctionPtrUpdateHandler : public Subscription<T>
	{
	public:
		using pass_t = typename Subscription<T>::pass_t;
		using func_t = FunctionPtr<void, pass_t>;

		FunctionPtrUpdateHandler();

		FunctionPtrUpdateHandler(const func_t& func);
		FunctionPtrUpdateHandler(const Watchable<T>& sub, const func_t& func);

		void setFunction(func_t&& func);
		void setFunction(const func_t& func);

		void clearFunction();

		virtual void onUpdate(pass_t value) override;

	private:
		func_t mFunc;
	};

	////////////////////////////////////

	template<typename ...args_t>
	FunctionEventHandler<args_t...>::FunctionEventHandler()
	{
	}

	template<typename ...args_t>
	FunctionEventHandler<args_t...>::FunctionEventHandler(func_t&& func)
	{
		mFunc = std::move(func);
	}

	template<typename ...args_t>
	FunctionEventHandler<args_t...>::FunctionEventHandler(const Event<args_t...>& evt, func_t&& func)
	{
		mFunc = std::move(func);
		bind(evt);
	}

	template<typename ...args_t>
	FunctionEventHandler<args_t...>::FunctionEventHandler(const func_t& func)
	{
		mFunc = func;
	}

	template<typename ...args_t>
	inline FunctionEventHandler<args_t...>::FunctionEventHandler(const Event<args_t...>& evt, const func_t& func)
	{
		mFunc = func;
		bind(evt);
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::setFunction(func_t&& func)
	{
		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::setFunction(const func_t& func)
	{
		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::clearFunction()
	{
		mFunc = func_t();
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::handleEvent(args_t ...arg)
	{
		if (mFunc && !blocked())
			mFunc(arg...);
	}

	////////////////////////////////////

	template<typename ...args_t>
	FunctionPtrEventHandler<args_t...>::FunctionPtrEventHandler()
	{
	}

	template<typename ...args_t>
	FunctionPtrEventHandler<args_t...>::FunctionPtrEventHandler(const func_t& func)
	{
		mFunc = func;
	}

	template<typename ...args_t>
	FunctionPtrEventHandler<args_t...>::FunctionPtrEventHandler(const Event<args_t...>& evt, const func_t& func)
	{
		mFunc = func;
		bind(evt);
	}

	template<typename ...args_t>
	void FunctionPtrEventHandler<args_t...>::setFunction(func_t&& func)
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionPtrEventHandler<args_t...>::setFunction(const func_t& func)
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionPtrEventHandler<args_t...>::clearFunction()
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = func_t();
	}

	template<typename ...args_t>
	void FunctionPtrEventHandler<args_t...>::handleEvent(args_t ...arg)
	{
		if (mFunc && !blocked())
			mFunc(arg...);
	}

	////////////////////////////////////

	template<typename T>
	FunctionUpdateHandler<T>::FunctionUpdateHandler()
	{
	}

	template<typename T>
	FunctionUpdateHandler<T>::FunctionUpdateHandler(func_t&& func)
	{
		mFunc = std::move(func);
	}

	template<typename T>
	FunctionUpdateHandler<T>::FunctionUpdateHandler(const Watchable<T>& sub, func_t&& func)
		: Subscription<T>(sub)
	{
		mFunc = std::move(func);
	}

	template<typename T>
	FunctionUpdateHandler<T>::FunctionUpdateHandler(const func_t& func)
	{
		mFunc = func;
	}

	template<typename T>
	FunctionUpdateHandler<T>::FunctionUpdateHandler(const Watchable<T>& sub, const func_t& func)
		: Subscription<T>(sub)
	{
		mFunc = func;
	}

	template<typename T>
	void FunctionUpdateHandler<T>::setFunction(func_t&& func)
	{
		if (isAttached())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename T>
	void FunctionUpdateHandler<T>::setFunction(const func_t& func)
	{
		if (isAttached())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename T>
	void FunctionUpdateHandler<T>::clearFunction()
	{
		if (isAttached())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = func_t();
	}

	template<typename T>
	void FunctionUpdateHandler<T>::onUpdate(pass_t value)
	{
		if (mFunc)
			mFunc(value); 
	}

	////////////////////////////////////

	template<typename T>
	FunctionPtrUpdateHandler<T>::FunctionPtrUpdateHandler()
	{
	}

	template<typename T>
	FunctionPtrUpdateHandler<T>::FunctionPtrUpdateHandler(const func_t& func)
	{
		mFunc = func;
	}

	template<typename T>
	FunctionPtrUpdateHandler<T>::FunctionPtrUpdateHandler(const Watchable<T>& sub, const func_t& func)
		: Subscription<T>(sub)
	{
		mFunc = func;
	}

	template<typename T>
	void FunctionPtrUpdateHandler<T>::setFunction(func_t&& func)
	{
		mFunc = std::move(func);
	}

	template<typename T>
	void FunctionPtrUpdateHandler<T>::setFunction(const func_t& func)
	{
		mFunc = std::move(func);
	}

	template<typename T>
	void FunctionPtrUpdateHandler<T>::clearFunction()
	{
		mFunc = func_t();
	}

	template<typename T>
	void FunctionPtrUpdateHandler<T>::onUpdate(pass_t value)
	{
		if (mFunc)
			mFunc(value);
	}
}

#endif // !_STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_