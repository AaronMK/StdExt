#ifndef _STD_EXT_CONCURRENT_TASK_H_
#define _STD_EXT_CONCURRENT_TASK_H_

#include "ThreadRunner.h"
#include "SysTask.h"

#include "../Concepts.h"
#include "../CallableTraits.h"

#include "../Chrono/Duration.h"

#include <atomic>
#include <cassert>
#include <functional>
#include <optional>
#include <type_traits>
#include <tuple>
#include <utility>
#include <variant>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  The running state of a task.
	 */
	enum class TaskState
	{
		/**
		 * @brief
		 *  The task has not been run nor is scheduled to be run.
		 */
		Dormant,

		/**
		 * @brief
		 *  The task has been placed into a scheduler, but has not stated execution.
		 */
		InQueue,

		/**
		 * @brief
		 *  The task is waiting on a timer or a condition.
		 */
		Blocked,

		/**
		 * @brief
		 *  The task is running.
		 */
		Running,

		/**
		 * @brief
		 *  Task has finished, and the results or exceptions thrown are ready.
		 */
		Finished
	};

	class Scheduler;

	class TaskBase
	{
		friend class SysTask;
		friend class Scheduler;
		friend class ThreadRunner;

	public:
		TaskBase();
		virtual ~TaskBase();

		TaskState state() const;

		void wait(const Chrono::Milliseconds& ms_timout = Chrono::Milliseconds(0.0));

		/**
		 * @brief
		 *  Resets the task to a dormant state.  Any results from previous runs or exceptions
		 *  thrown will be disregarded.  This should be considered a way to recycle tasks.
		 * 
		 * @details
		 *  Overrides should call this base impelmentation then do their own cleanup;
		 */
		virtual void reset();

		virtual void run_task() = 0;

	protected:
		std::exception_ptr mException;
		TaskState          mState;

		void run_as_thread();

		std::variant<std::monostate, ThreadRunner, SysTask> mRunner;
	};

	template<typename ret_t, typename... args_t>
	class Task : public TaskBase
	{
		friend class Scheduler;

	private:
		static constexpr bool has_args   = (sizeof...(args_t) > 0);
		static constexpr bool has_return = !std::is_void_v<ret_t>;

		using task_t = Task<ret_t, args_t...>;

		using ret_container_t = std::conditional_t<
			has_return,
			std::optional<ret_t>, std::optional<std::monostate>
		>;

		using arg_container_t =  std::conditional_t<
			has_args, std::optional< std::tuple<args_t...> >, std::monostate
		>;

		ret_container_t mResult;
		arg_container_t mArgs;

		void run_task_impl()
			requires (has_return && has_args)
		{
			mResult.emplace(
				std::apply(
					[this](args_t... args)
					{
						return run(std::forward<args_t>(args)...);
					},
					*mArgs
				)
			);
		}
		
		void run_task_impl()
			requires (has_return && !has_args)
		{
			mResult.emplace( run() );
		}
		
		void run_task_impl()
			requires (!has_return && has_args)
		{
			std::apply(
				[this](args_t... args)
				{
					run(std::forward<args_t>(args)...);
				},
				*mArgs
			);
		}
		
		void run_task_impl()
			requires (!has_return && !has_args)
		{
			run();
		}

	protected:
		void run_task() override
		{
			try
			{
				mState = TaskState::Running;
				run_task_impl();
			}
			catch ( ... )
			{
				mException = std::current_exception();
			}
			
			mState = TaskState::Finished;
		}

		virtual ret_t run(args_t... args) = 0;

	public:
		Task()
		{
		}

		virtual ~Task()
		{
			try
			{
				TaskBase::wait();
			}
			catch(...)
			{
			}
		}

		ret_t runAsThread(args_t... args)
		{
			assert( std::holds_alternative<std::monostate>(mRunner) );

			if constexpr ( has_args )
				mArgs.emplace(std::forward<args_t>(args)...);

			mRunner.emplace<ThreadRunner>(this);
		}

		void reset() override
		{
			TaskBase::reset();

			if constexpr ( has_return )
				mResult.reset();

			if constexpr ( has_args )
				mArgs.reset();
		}

		ret_t get(const Chrono::Milliseconds& ms_timout = Chrono::Milliseconds(0.0))
		{
			wait(ms_timout);

			if ( mException )
				std::rethrow_exception(mException);

			if constexpr ( has_return )
				return *mResult;
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_TASK_H_
