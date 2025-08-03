#include <cassert>
#include <chrono>
#include <coroutine>
#include <thread>

#include <StdExt/Concurrent/PredicatedCondition.h>

#include <StdExt/Concurrent/Utility.h>
#include <StdExt/FunctionTraits.h>
#include <StdExt/Test/Test.h>


using namespace StdExt::Concurrent;
using namespace StdExt::Test;
using namespace StdExt;

using namespace std::chrono;

template<typename T>
concept CotoutineReturn = requires (T obj)
{
	typename T::promise_type;
};

template<typename return_t, typename... args_t>
class Task;

template<typename return_t, typename... args_t>
class Task<return_t(args_t...)>
{
public:
	struct promise_type
	{
		std::suspend_never initial_suspend()
		{
			return {};
		}

		std::suspend_never final_suspend() noexcept
		{
			return {};
		}

		void unhandled_exception() {}

		template<typename... construct_args>
		Task<return_t(args_t...)> get_return_object(construct_args&&... args)
		{
			return Task<return_t(args_t...)>{};
		};

		template<typename... construct_args>
		void return_value(construct_args&&... args)
		{
			return;
		}
	};
};

constinit static PredicatedCondition pred_cond;
constinit static int cond_int_val{0};

static Task<int(int)> addTwo(int left)
{
	pred_cond.wait(
		[&]()
		{
			return (cond_int_val >= 1);
		}
	);

	co_return left + 2;
}

void testPredicated()
{
	auto coro = addTwo(2);

	using func_traits = Function<&addTwo>::return_type;

	{
		std::binary_semaphore bin_sem{1};

		bool thread_2_got_lock = false;

		auto thread_2 = [&](SemLock held_lock)
		{
			assert(held_lock);

			std::this_thread::sleep_for( milliseconds(500) );
			thread_2_got_lock = true;
		};

		std::jthread thread_1(
			[&]() mutable
			{
				SemLock lock(bin_sem);
				std::jthread passed_thread(thread_2, std::move(lock));

				lock.acquire(bin_sem);
				testForResult(
					"SemLock: Ownership of std::binary_semaphore is transferred.",
					true, thread_2_got_lock
				);
			}
		);
	}
	constinit static std::counting_semaphore<2> count_sem{2};
}