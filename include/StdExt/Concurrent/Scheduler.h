#ifndef _STD_EXT_CONCURRENT_SCHEDULER_H_
#define _STD_EXT_CONCURRENT_SCHEDULER_H_

#include "../StdExt.h"

#include "Task.h"

#include "../String.h"

#if defined(STD_EXT_APPLE)
#	include <dispatch/dispatch.h>
#elif defined(STD_EXT_WIN32)
#	include <concrt.h>
#elif defined(STD_EXT_GCC)
#endif

namespace StdExt::Concurrent
{
	enum class SchedulerType
	{
		SERIAL,
		PARALLEL
	};

	/**
	 * @note
	 *  At present, the GCC implementation of the parallel scheduler is very "stopgap",
	 *  and just passes tasks to std::async().
	 */
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

			if constexpr ( !std::is_void_v<ret_t> )
				task->mResult.reset();

			addTaskBase(task);
		}

		template<typename ret_t, typename... args_t>
		void addTask(Task<ret_t, args_t...>& task, args_t... args)
		{
			addTask(&task, std::forward<args_t>(args)...);
		}

	protected:
		void addTaskBase(TaskBase* task);

#if defined (STD_EXT_APPLE)
		dispatch_queue_t mDispatchQueue;
#elif defined (STD_EXT_WIN32)

		static void runTask(void* task_ptr);
	
		Concurrency::Scheduler* mScheduler;
		String                  mName;
#elif defined (STD_EXT_GCC)
		class SerialExecutor;

		std::unique_ptr<SerialExecutor> mSerialExecutor;
		String                          mName;
#endif
	};
}

#endif // _STD_EXT_CONCURRENT_SCHEDULER_H_
