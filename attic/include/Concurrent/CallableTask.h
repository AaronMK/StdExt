#ifndef _STD_EXT_CONCURRENT_CALLABLE_TASK_H_
#define _STD_EXT_CONCURRENT_CALLABLE_TASK_H_

#include "Task.h"

#include "../CallableTraits.h"
#include "../Concepts.h"
#include "../Type.h"

#include <type_traits>

namespace StdExt::Concurrent
{
	namespace detail
	{
		template<typename callable_t, typename ret_t, typename... args_t>
		class CallableTaskForward : public Task<ret_t, args_t...>
		{
			static_assert( CallableWith<callable_t, ret_t, args_t...> );
			
		private:
			callable_t mCallable;

		public:
			CallableTaskForward(callable_t&& callable)
				: mCallable( std::move(callable) )
			{
			}

			CallableTaskForward(const callable_t& callable)
				: mCallable( std::move(callable) )
			{
			}

		protected:
			ret_t run(args_t... args) override
			{
				if constexpr ( std::is_void_v<ret_t> )
					mCallable(std::forward<args_t>(args)...);
				else
					return mCallable(std::forward<args_t>(args)...);
			}
		};

		template<typename callable_t>
		using callable_task_forward_t = CallableTraits<callable_t>::template forward<detail::CallableTaskForward, callable_t>;
	}

	template<typename callable_t>
	class CallableTask : public detail::callable_task_forward_t<callable_t>
	{
	private:
		using details_callable_base_t = CallableTraits<callable_t>::template forward<detail::CallableTaskForward, callable_t>;
	
	public:
		CallableTask(callable_t&& func)
			: detail::callable_task_forward_t<callable_t>( std::move(func) )
		{
		}
		
		CallableTask(const callable_t& func)
			: detail::callable_task_forward_t<callable_t>( std::move(func) )
		{
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_CALLABLE_TASK_H_
