#ifndef _STD_EXT_CONCURRENT_SCHEDULER_H_
#define _STD_EXT_CONCURRENT_SCHEDULER_H_

#include "../StdExt.h"

#include "Task.h"

#include "../String.h"

#if defined (STD_EXT_APPLE)
#	include <dispatch/dispatch.h>
#endif

namespace StdExt::Concurrent
{
	enum class SchedulerType
	{
		SERIAL,
		PARALLEL
	};

	class STD_EXT_EXPORT Scheduler
	{
	public:
		Scheduler(SchedulerType stype = SchedulerType::PARALLEL);
		Scheduler(const String& name, SchedulerType stype = SchedulerType::PARALLEL);

		virtual ~Scheduler();

		template<typename ret_t, typename... args_t>
		void addTask(Task<ret_t, args_t...>* task, args_t... args)
		{
			if constexpr ( sizeof...(args_t) > 0 )
			{
				task->mArgs = std::tuple<args_t...>(
					std::forward<args_t>(args)...
				);
			}

			addTaskBase(task);
		}

	protected:
		void addTaskBase(TaskBase* task);

#if defined (STD_EXT_APPLE)
		dispatch_queue_t mDispatchQueue;
#endif
	};
}

#endif // _STD_EXT_CONCURRENT_SCHEDULER_H_
