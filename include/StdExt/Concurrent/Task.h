#ifndef _STD_EXT_CONCURRENT_TASK_H_
#define _STD_EXT_CONCURRENT_TASK_H_

#include "../Chrono/Duration.h"

#include "../Concepts.h"

#if defined(STD_EXT_APPLE)
#	include <dispatch/dispatch.h>
#elif defined(STD_EXT_WIN32)
#	include <agents.h>
#elif defined(STD_EXT_GCC)
#	include <future>
#endif

#include <atomic>
#include <cassert>
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

		TaskState state() const;

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
#elif defined (STD_EXT_GCC)
		std::future<void> mFuture;
#endif
	};

	template<typename ret_t, typename... args_t>
	class Task;

	template<Void ret_t, typename... args_t>
	class Task<ret_t, args_t...> : public TaskBase
	{
		friend class Scheduler;
		static constexpr bool has_args    = sizeof...(args_t) > 0;

	public:
		void wait(Chrono::Milliseconds timeout = Chrono::Milliseconds(0))
		{
			TaskBase::internalWait(timeout);
			assert( TaskState::Finished == mTaskState );
		}

	protected:
		virtual void run(args_t... args) = 0;

	private:
		using arg_obj_t = std::conditional_t<
			sizeof...(args_t) == 0,
			std::monostate,
			std::optional<std::tuple<args_t...>>
		>;

		arg_obj_t mArgs;

		void schedulerRun() override
		{
			if constexpr ( has_args )
			{
				assert( mArgs.has_value() );
				std::apply(
					[this](args_t... args)
					{
						run(std::forward<args_t>(args)...);
					},
					*mArgs
				);
			}
			else
			{
				run();
			}
		}
	};

	template<NonVoid ret_t, typename... args_t>
	class Task<ret_t, args_t...> : public TaskBase
	{
		friend class Scheduler;
		static constexpr bool has_args = sizeof...(args_t) > 0;

	public:
		ret_t& get(Chrono::Milliseconds timeout = Chrono::Milliseconds(0))
		{
			TaskBase::internalWait(timeout);
			assert( TaskState::Finished == mTaskState && mResult.has_value());

			return *mResult;
		}

	protected:
		virtual ret_t run(args_t... args) = 0;

	private:
		using arg_obj_t = std::conditional_t<
			sizeof...(args_t) == 0,
			std::monostate,
			std::optional<std::tuple<args_t...>>
		>;

		arg_obj_t mArgs;
		std::optional<ret_t> mResult;

		void schedulerRun() override
		{
			if constexpr ( has_args )
			{
				assert( mArgs.has_value() );
				mResult = std::apply(
					[this](args_t... args)
					{
						return run(std::forward<args_t>(args)...);
					},
					*mArgs
				);
			}
			else
			{
				mResult = run();
			}
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_TASK_H_
