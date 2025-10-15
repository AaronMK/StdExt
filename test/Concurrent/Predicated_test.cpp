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

enum class PredEndType
{
	None,
	Success,
	Destroyed,
	Timeout
};

void testPredicated()
{
	std::array<PredEndType, 5> wait_results;
	wait_results.fill(PredEndType::None);

	std::atomic<bool> cond_triggered = false;
	std::atomic<size_t> start_count = 0;

	PredicatedCondition pred_cond;

	auto wait_on_index = [&](size_t index)
	{
		if (++start_count == 4)
			pred_cond.trigger();

		try
		{
			int timeout = 0;

			if ( 3 == index )
				timeout = 1;

			if ( 4 == index )
				timeout = 3;

			pred_cond.wait(
				[&]() -> bool
				{
					return cond_triggered && (index % 2 == 0) && (4 != index);
				},
				[&]()
				{
					wait_results[index] = PredEndType::Success;
				},
				Chrono::Seconds(timeout)
			);
		}
		catch (const object_destroyed&)
		{
			wait_results[index] = PredEndType::Destroyed;
		}
		catch (const time_out&)
		{
			wait_results[index] = PredEndType::Timeout;
		}
	};

	std::array<std::thread, 5> wait_threads{
		std::thread(wait_on_index, 0),
		std::thread(wait_on_index, 1),
		std::thread(wait_on_index, 2),
		std::thread(wait_on_index, 3),
		std::thread(wait_on_index, 4)
	};
	
	pred_cond.wait([&]() { return (start_count >= 4); } );
	std::this_thread::sleep_for(seconds(2));

	pred_cond.trigger(
		[&]()
		{
			cond_triggered = true;
		}, 2
	);

	pred_cond.destroy();

	for (auto& t : wait_threads)
		t.join();

	testForResult(
		"PredicatedCondition: Wait with satisfied precondition had its action run.",
		wait_results[0], PredEndType::Success
	);

	testForResult(
		"PredicatedCondition: Wait with no timeout and unsatisfied condition "
		"had an object_destroyed exception thrown.",
		wait_results[1], PredEndType::Destroyed
	);

	testForResult(
		"PredicatedCondition: Multiple conditions that are satisfied will be woken.",
		wait_results[2], PredEndType::Success
	);

	testForResult(
		"PredicatedCondition: Wait will timeout before a destroy call if "
		"the timeout is shorter.",
		wait_results[3], PredEndType::Timeout
	);

	testForResult(
		"PredicatedCondition: Pre-condition with a timeout locnger than the time to "
		"a destroy call will still raise an object_destroyed exception.",
		wait_results[4], PredEndType::Destroyed
	);
}