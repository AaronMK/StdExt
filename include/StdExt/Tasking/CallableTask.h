#ifndef _STD_EXT_TASKING_CALLABLE_TASK_H_
#define _STD_EXT_TASKING_CALLABLE_TASK_H_

#include "Task.h"

#include "../CallableTraits.h"

namespace StdExt::Tasking
{
	template<HasSignature<void> callable_t>
	class CallableTask : public Task
	{
	private:
		callable_t mCallable;

	public:
		CallableTask(callable_t&& func)
			: mCallable( std::forward<callable_t>(func) )
		{
		}
		
		CallableTask(const callable_t& func)
			: mCallable(func)
		{
		}

		void run_task() override
		{
			mCallable();
		}
	};
}
#endif // !_STD_EXT_TASKING_CALLABLE_TASK_H_