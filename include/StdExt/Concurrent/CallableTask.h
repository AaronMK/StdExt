#ifndef _STD_EXT_CONCURRENT_CALLABLE_TASK_H_
#define _STD_EXT_CONCURRENT_CALLABLE_TASK_H_

#include "Task.h"

#include "../FunctionTraits.h"
#include "../Concepts.h"
#include "../Type.h"

#include <type_traits>

namespace StdExt::Concurrent
{
	template<typename callable_t, typename ret_t, typename... args_t>
	class CallableTask;

	template<typename callable_t, typename ret_t, typename... args_t>
	class CallableTask<callable_t, ret_t(args_t...)> : public Task<ret_t(args_t...)>
	{
	private:
		callable_t mCallable;

	public:
		CallableTask(callable_t&& callable)
			: mCallable( std::forward<callable_t>(callable) )
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
	auto makeTask(callable_t&& func)
	{
		using callable_param_t = Type<callable_t>::stripped_t;
		using result_t = Function<&callable_t::operator()>::template forward<CallableTask, callable_param_t>;

		return result_t(std::forward<callable_param_t>(func));
	}
}

#endif // !_STD_EXT_CONCURRENT_CALLABLE_TASK_H_
