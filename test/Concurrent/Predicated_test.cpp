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

void testPredicated()
{
	std::array<bool, 4> condition_met;
	condition_met.fill(false);

	std::atomic<bool> cond_triggered = false;
	std::atomic<size_t> start_count = 0;

	PredicatedCondition pred_cond;

	auto wait_on_index = [&](size_t index)
	{
		if (++start_count == 4)
			pred_cond.trigger();

		try
		{
			pred_cond.wait(
				[&]() -> bool
				{
					return cond_triggered && (index % 2 == 0);
				},
				[&]()
				{
					condition_met[index] = true;
				}
			);
		}
		catch(const object_destroyed& od)
		{
			int i = 1;
		}
	};

	std::array<std::thread, 4> wait_threads{
		std::thread(wait_on_index, 0),
		std::thread(wait_on_index, 1),
		std::thread(wait_on_index, 2),
		std::thread(wait_on_index, 3)
	};
	
	std::this_thread::sleep_for(seconds(1));

	pred_cond.wait([&]() { return (start_count >= 4); } );
	pred_cond.trigger([&]() { cond_triggered = true; }, 2);

	std::this_thread::sleep_for(seconds(5));
	pred_cond.destroy();

	for (auto& t : wait_threads)
		t.join();
}