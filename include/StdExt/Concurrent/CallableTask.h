#ifndef _STD_EXT_CONCURRENT_CALLABLE_TASK_H_
#define _STD_EXT_CONCURRENT_CALLABLE_TASK_H_

#include "Task.h"

#include "../Concepts.h"

namespace StdExt::Concurrent
{
	template<CallableWith<void> callable_t>
	class CallableTask : public Task
	{
	private:
		callable_t mCallable;

	public:
		CallableTask(callable_t&& callable)
			: Task(), mCallable( std::move(callable) )
		{
		}
		CallableTask(const callable_t& callable)
			: Task(), mCallable( callable )
		{
		}

	protected:
		virtual void run() override
		{
			mCallable();
		}
	};

	template<CallableWith<void> callable_t>
	auto makeTask(callable_t&& callable)
	{
		return CallableTask<callable_t>(std::move(callable));
	}

	template<CallableWith<void> callable_t>
	auto makeTask(const callable_t& callable)
	{
		return CallableTask<callable_t>(callable);
	}
}

#endif // !_STD_EXT_CONCURRENT_CALLABLE_TASK_H_