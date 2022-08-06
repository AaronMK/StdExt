#ifndef _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_
#define _STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_

#include "Event.h"
#include "Watchable.h"
#include "CallableHandler.h"
#include "Subscription.h"

#include "../Exceptions.h"
#include "../FunctionPtr.h"

#include <functional>

namespace StdExt::Signals
{
	template<typename  ...args_t>
	using FunctionEventHandler = CallableEventHandler<std::function<void(args_t...)>, args_t...>;

	template<typename T>
	using FunctionUpdateHandler = CallableUpdateHandler<std::function<void(T)>, T>;
}

#endif // !_STD_EXT_SIGNALS_FUNCTION_HANDLERS_H_