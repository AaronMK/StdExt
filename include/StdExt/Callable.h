#ifndef _STD_EXT_CALLABLE_H_
#define _STD_EXT_CALLABLE_H_

#include "CallableTraits.h"

namespace StdExt
{
	template<typename ret_t, typename... args_t>
	class Callable
	{
	public:
		Callable() {}
		virtual ~Callable() {}

		ret_t operator()(args_t... args) const
		{
			if constexpr ( std::is_void_v<ret_t> )
				run(std::forward<args_t>(args)...);
			else
				return run(std::forward<args_t>(args)...);
		}

		template<typename callable_t>
		class Implementation : public Callable<ret_t, args_t...>
		{
		private:
			callable_t mCallable;
		public:
			Implementation(const callable_t& callable)
				requires CopyConstructable<callable_t>
				: mCallable(callable)
			{
			}

			Implementation(callable_t&& callable)
				requires MoveConstructable<callable_t>
				: mCallable( std::move(callable) )
			{
			}

		protected:
			ret_t run(args_t... args) const override
			{
				
			if constexpr ( std::is_void_v<ret_t> )
				mCallable(std::forward<args_t>(args)...);
			else
				return mCallable(std::forward<args_t>(args)...);
			}
		};

	protected:
		virtual ret_t run(args_t... args) const = 0;
	};

	template<typename callable_t>
	auto makeCallable(callable_t&& func)
	{
		using call_t = CallableTraits<callable_t>::template forward<Callable>;
		using imp_t  = call_t::template Implementation<callable_t>;

		return imp_t(std::forward<callable_t>(func));
	}
}

#endif // !_STD_EXT_CALLABLE_H_