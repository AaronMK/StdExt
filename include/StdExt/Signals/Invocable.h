#ifndef _STD_EXT_SIGNALS_INVOKABLE_
#define _STD_EXT_SIGNALS_INVOKABLE_

#include "Event.h"

namespace StdExt::Signals
{
	/**
	 * @brief
	 *  Base class for events that can be externally invoked.
	 */
	template <typename ...args_t>
	class Invocable : public Event<args_t...>
	{
	public:
		using base_t = Event<args_t...>;
		using my_t = Invocable<args_t...>;

		my_t& operator=(const my_t&) const = delete;
		Invocable(const my_t&) = delete;

		my_t& operator=(my_t&&) = default;
		Invocable(my_t&&) = default;

		Invocable() = default;

		/**
		 * @brief
		 *  The default implementation simply calls the internally protected notify().
		 */
		virtual void invoke(args_t ...args)
		{
			base_t::notify(std::forward<args_t>(args)...);
		}
	};
}

#endif // !_STD_EXT_SIGNALS_INVOKABLE_
