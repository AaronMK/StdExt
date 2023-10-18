#ifndef _STD_EXT_CONCURRENT_CALLABLE_TASK_H_
#define _STD_EXT_CONCURRENT_CALLABLE_TASK_H_

#include "Task.h"

#include "../CallableTraits.h"
#include "../Concepts.h"

#include <type_traits>

namespace StdExt::Concurrent
{
	template<typename callable_t>
	class CallableTask : public CallableTraits<callable_t>::forward<Task>
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

	template<typename callable_t>
	auto makeTask(callable_t&& func)
	{
		return CallableTask<callable_t>(func);
	}
}

#endif // !_STD_EXT_CONCURRENT_CALLABLE_TASK_H_
