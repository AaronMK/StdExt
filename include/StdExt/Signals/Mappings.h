#ifndef _STD_EXT_SIGNALS_MAPPINGS_H_
#define _STD_EXT_SIGNALS_MAPPINGS_H_

#include "FunctionHandlers.h"
#include "Watchable.h"
#include "Settable.h"
#include "Event.h"

#include "../Type.h"
#include "../Any.h"

#include "../Collections/Vector.h"

#include <memory>

namespace StdExt
{
	class Mappings
	{
	private:
		Collections::AdvVector<Any> mObjects;

	public:
		template<typename ...args_t>
		void map(Event<args_t...>& evt, std::function<void(args_t...)>&& func)
		{
			using func_t = std::function<void(...args_t)>;
			using handler_t = FunctionEventHandler<...args_t>;

			Any funcHandler = make_any<handler_t>(std::move(func));

			handler_t* funcHandlerPtr = funcHandler.cast<handler_t>();
			funcHandlerPtr->bind(evt);

			mObjects.emplace_back(std::move(funcHandler));
		}

		template<typename T>
		void map(const std::shared_ptr<Watchable<T>>& watched, const std::shared_ptr<Settable<T>>& setter)
		{
			using func_t = std::function<void(const T&)>;
			using handler_t = FunctionUpdateHandler<T>;

			setter->setValue(watched->value());
			Any funcHandler = make_any<handler_t>(
				[=](const T& val) mutable
				{
					setter->setValue(val);
				}
			);
			handler_t* handler = funcHandler.cast<handler_t>();
			
			handler->attach(watched);

			mObjects.emplace_back(std::move(funcHandler));
		}

		template<typename T>
		void map(const std::shared_ptr<Watchable<T>>& watched, std::function<void(const T&)>&& func)
		{
			using func_t = std::function<void(const T&)>;
			using handler_t = FunctionUpdateHandler<T>;

			func(watched->value());
			Any funcHandler = make_any<handler_t>(std::move(func));
			handler_t* handler = funcHandler.cast<handler_t>();
			
			handler->attach(watched);

			mObjects.emplace_back(std::move(funcHandler));
		}
	};
}


#endif // !_STD_EXT_SIGNALS_MAPPINGS_H_