#ifndef _STD_EXT_CONCURRENT_TASK_H_
#define _STD_EXT_CONCURRENT_TASK_H_

#include "../Chrono/Duration.h"

#if defined(STD_EXT_APPLE)
#	include <dispatch/dispatch.h>
#elif defined(STD_EXT_WIN32)
#	include <agents.h>
#endif

#include <atomic>
#include <optional>
#include <type_traits>
#include <tuple>
#include <utility>
#include <variant>

namespace StdExt::Concurrent
{
	enum class TaskState
	{
		Dormant,
		InQueue,
		Running,
		Finished
	};

	class STD_EXT_EXPORT TaskBase
	{
		friend class Scheduler;

	public:
		TaskBase();
		virtual ~TaskBase();

	protected:
		void internalWait(Chrono::Milliseconds timeout = Chrono::Milliseconds(0));

		std::atomic<TaskState> mTaskState;

	private:
		virtual void schedulerRun() = 0;

		std::exception_ptr mException;

#if defined(STD_EXT_APPLE)
		dispatch_block_t   mDispatchBlock;
#elif defined (STD_EXT_WIN32)
		concurrency::event mEvent;
#endif
	};

	template<typename ret_t, typename... args_t>
	class Task : TaskBase
	{
		friend class Scheduler;

		static constexpr bool is_ret_void = std::is_same_v<void, ret_t>;
		static constexpr bool has_args    = sizeof...(args_t) > 0;

	public:
		Task()
		{
		}

		virtual ~Task()
		{
		}

		void operator()(args_t... args)
			requires ( is_ret_void )
		{
			run(std::forward<args_t>(args)...);
		}

		ret_t operator()(args_t... args)
			requires ( !is_ret_void )
		{
			return run(std::forward<args_t>(args)...);
		}

		void wait(Chrono::Milliseconds timeout = Chrono::Milliseconds(0))
			requires ( is_ret_void )
		{
			TaskBase::internalWait(timeout);
			assert( TaskState::Finished == mTaskState && mResult.has_value());

			mTaskState = TaskState::Dormant;
		}

		ret_t get(Chrono::Milliseconds timeout = Chrono::Milliseconds(0))
			requires ( !is_ret_void )
		{
			TaskBase::internalWait(timeout);
			assert( TaskState::Finished == mTaskState && mResult.has_value());

			mTaskState = TaskState::Dormant;

			ret_t ret(std::move(*mResult));
			mResult.reset();

			return ret;
		}

	protected:
		virtual ret_t run(args_t... args) = 0;

	private:
		using arg_obj_t = std::conditional<
			sizeof...(args_t) == 0, std::monostate, std::tuple<args_t...>
		>;

		using ret_obj_t = std::conditional<
			is_ret_void, std::monostate, ret_t
		>;

		std::optional<arg_obj_t> mArgs;
		std::optional<ret_obj_t> mResult;

		void schedulerRun() override
		{
			if constexpr ( has_args )
			{
				assert( mArgs.has_value() );

				if constexpr ( is_ret_void )
					std::apply((*this), mArgs);
				else
					mResult = std::apply((*this), mArgs);
			}
			else
			{
				if constexpr ( is_ret_void )
					run();
				else
					mResult = run();
			}
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_TASK_H_
