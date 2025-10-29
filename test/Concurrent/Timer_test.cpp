#include <StdExt/Test/Test.h>

#include <StdExt/Chrono/Duration.h>
#include <StdExt/Chrono/Stopwatch.h>
#include <StdExt/Collections/Vector.h>
#include <StdExt/Concurrent/Timer.h>

#include <thread>

using namespace StdExt;
using namespace StdExt::Chrono;
using namespace StdExt::Concurrent;
using namespace StdExt::Test;

void testTimer()
{
	{
		Stopwatch stopwatch;
		uint32_t  timer_count = 0;

		constexpr auto tick_period = Milliseconds(500);
		constexpr auto total_time = Milliseconds(2250);
		bool timing_accurate = true;

		Collections::Vector<double, 4> trigger_times;

		CallableTimer timer(
			[&]()
			{
				++timer_count;
				double total_ms    = Milliseconds(stopwatch.time()).count();
				double expected_ms = timer_count * tick_period.count();

				if ( !approxEqual(total_ms, expected_ms, 0.05f) )
					timing_accurate = false;

				trigger_times.emplace_back(total_ms);
			}
		);

		timer.start(tick_period);
		stopwatch.start();
		std::this_thread::sleep_for(total_time);
		timer.stop();

		testForResult<bool>(
			"Timer: Triggered at expected intervals.",
			true, timing_accurate
		);
		
		testForResult<uint32_t>(
			"Timer: Triggered the expected number of times.",
			4, timer_count
		);
	}

	{
		uint32_t  timer_count = 0;
		constexpr auto one_shot_time = Milliseconds(500);

		{
			CallableTimer timer(
				[&]()
				{
					++timer_count;
				}
			);

			timer.oneShot(one_shot_time);
			std::this_thread::sleep_for(Milliseconds(250));
			timer.stop();
			std::this_thread::sleep_for(Milliseconds(500));

			testForResult<uint32_t>(
				"Timer: OneShot is not triggered when stopped before timeout.",
				0, timer_count
			);
		}

		testForResult<uint32_t>(
			"Timer: OneShot is not triggered when on destruction.",
			0, timer_count
		);
	}

	{
		uint32_t  timer_count = 0;
		constexpr auto one_shot_time = Milliseconds(250);

		CallableTimer timer(
			[&]()
			{
				++timer_count;
			}
		);

		timer.oneShot(one_shot_time);
		std::this_thread::sleep_for(one_shot_time * 3);

		testForResult<uint32_t>(
			"Timer: OneShot is only triggered once.",
			1, timer_count
		);
	}
}