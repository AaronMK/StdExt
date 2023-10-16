#ifndef _STD_EXT_CONCURRENT_CALLABLE_TASK_H_
#define _STD_EXT_CONCURRENT_CALLABLE_TASK_H_

#include "Task.h"

#include "../Concepts.h"

namespace StdExt::Concurrent
{
	template<typename callable_t, typename ret_t, typename... args_t>
		requires ( CallableWith<callable_t, ret_t, args_t...> && Class<callable_t> )
	class CallableTask : public Task<ret_t, args_t...>
	{
	private:
		callable_t mCallable;

	public:
		CallableTask(callable_t&& callable)
			: Task(), mCallable( std::move(callable) )
		{
		}

		CallableTask(const callable_t& callable)
			requires( !ReferenceType<callable_t> )
			: Task(), mCallable( callable )
		{
		}
	protected:
		ret_t run(args_t... args) override
		{
			mCallable(std::forward<args_t>(args)...);
		}
	};
	
	template<typename callable_t, typename ret_t, typename... args_t>
		requires ( CallableWith<callable_t, ret_t, args_t...> && Class<callable_t> )
	auto makeTask(callable_t&& callable)
	{
		return CallableTask<callable_t>(std::move(callable));
	}

	template<typename callable_t, typename ret_t, typename... args_t>
		requires ( CallableWith<callable_t, ret_t, args_t...> && Class<callable_t> )
	auto makeTask(const callable_t& callable)
	{
		return CallableTask<callable_t>(callable);
	}
}

#endif // !_STD_EXT_CONCURRENT_CALLABLE_TASK_H_
