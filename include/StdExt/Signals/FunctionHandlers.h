#ifndef _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_
#define _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_

#include "Event.h"
#include "Watchable.h"

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
		FunctionEventHandler(const func_t& func);

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
		using func_t = std::function<void(T)>;

		FunctionUpdateHandler();
		FunctionUpdateHandler(func_t&& func);
		FunctionUpdateHandler(const func_t& func);

		void setFunction(func_t&& func);
		void setFunction(const func_t& func);

		void clearFunction();

		virtual void onUpdate(const T& value) override;

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
	FunctionEventHandler<args_t...>::FunctionEventHandler(const func_t& func)
	{
		mFunc = func;
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::setFunction(func_t&& func)
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::setFunction(const func_t& func)
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::clearFunction()
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = func_t();
	}

	template<typename ...args_t>
	void FunctionEventHandler<args_t...>::handleEvent(args_t ...arg)
	{
		if (mFunc)
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
	FunctionUpdateHandler<T>::FunctionUpdateHandler(const func_t& func)
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
	void FunctionUpdateHandler<T>::onUpdate(const T& value)
	{
		if (mFunc)
			mFunc(value); 
	}
}

#endif // !_STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_