#ifndef _STD_EXT_CONCURRENT_CALLABLE_TASK_H_
#define _STD_EXT_CONCURRENT_CALLABLE_TASK_H_

#include "Task.h"

#include "../CallableTraits.h"
#include "../Concepts.h"
#include "../Type.h"

#include <type_traits>

namespace StdExt::Concurrent
{
	template<typename callable_t, typename ret_t, typename... args_t>
	class CallableTask : public Task<ret_t, args_t...>
	{
	private:
		callable_t mCallable;

	public:
		CallableTask(callable_t&& callable)
			: mCallable( std::move(callable) )
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
			if constexpr ( std::is_void_v<ret_t> )
				mCallable(std::forward<args_t>(args)...);
			else
				return mCallable(std::forward<args_t>(args)...);
		}
	};

	template<typename callable_t>
	auto makeTask(callable_t func)
	{
		return CallableTraits<callable_t>::forward<CallableTask, callable_t>(
			std::forward<callable_t>(func)
		);
	}
}

#endif // !_STD_EXT_CONCURRENT_CALLABLE_TASK_H_
