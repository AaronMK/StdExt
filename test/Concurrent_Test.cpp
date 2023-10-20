#include <StdExt/Chrono/Duration.h>
#include <StdExt/Chrono/Stopwatch.h>

#include <StdExt/Concurrent/CallableTask.h>
#include <StdExt/Concurrent/Mutex.h>
#include <StdExt/Concurrent/PredicatedCondition.h>
#include <StdExt/Concurrent/Producer.h>
#include <StdExt/Concurrent/Scheduler.h>
#include <StdExt/Concurrent/Timer.h>
#include <StdExt/Concurrent/Wait.h>

#include <StdExt/Signals/FunctionHandlers.h>
#include <StdExt/Signals/Invokable.h>

#include <StdExt/Test/Test.h>

#include <array>
#include <chrono>
#include <string>
#include <thread>
#include <iostream>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Concurrent;
using namespace StdExt::Chrono;

using namespace std;
using namespace std::chrono;

void testConcurrent()
{
	Scheduler scheduler(u8"testConcurrent() Scheduler");

	auto timeRelativeError = [](Nanoseconds expected, Nanoseconds observed)
	{
		return relative_difference(expected.count(), observed.count());
	};

	{
		int test_int = 0;
		auto test_task = makeTask(
			[&]()
			{
				test_int = 1;
			}
		);

		testForResult(
			"Task that has not started is in the dormant state.",
			TaskState::Dormant, test_task.state()
		);

		scheduler.addTask(test_task);
		test_task.wait();

		testForResult(
			"Task that has finished is in the Finished state.",
			TaskState::Finished, test_task.state()
		);

		testForResult(
			"Task in finished state has run.",
			1, test_int
		);
	}

	{
		auto test_task = makeTask(
			[&]()
			{
				return 1;
			}
		);

		testForResult(
			"Task with return value that has not started is in the dormant state.",
			TaskState::Dormant, test_task.state()
		);

		scheduler.addTask(test_task);
		test_task.get();

		testForResult(
			"Task with return value in finished state returns expected value.",
			1, test_task.get()
		);

		testForResult(
			"Task that has finished is in the Finished state.",
			TaskState::Finished, test_task.state()
		);
	}

	{
		auto test_task = makeTask(
			[&](int i)
			{
				return i;
			}
		);

		testForResult(
			"Task with return and arguments that has not started is in the dormant state.",
			TaskState::Dormant, test_task.state()
		);

		scheduler.addTask(test_task, 1);
		test_task.get();

		testForResult(
			"Task with return value in finished state returns expected value.",
			1, test_task.get()
		);

		testForResult(
			"Task that has finished is in the Finished state.",
			TaskState::Finished, test_task.state()
		);
	}

	{
		int test_int = 0;
		auto test_task = makeTask(
			[&]()
			{
				std::this_thread::sleep_for(Milliseconds(500));
				test_int = 1;
			}
		);

		scheduler.addTask(test_task);

		testForException<time_out>(
			"Waiting for an insufficient amount of time for a task raises a timeout exception.",
			[&]()
			{
				test_task.wait( Milliseconds(100) );
			}
		);

		testForResult<int>(
			"A wait after a failed wait can succeed.", 1,
			[&]()
			{
				test_task.wait();
				return test_int;
			}
		);
	}

	{
		auto test_task = makeTask(
			[&]()
			{
				std::this_thread::sleep_for(Milliseconds(500));
				return 1;
			}
		);

		scheduler.addTask(test_task);

		testForException<time_out>(
			"Waiting for an insufficient amount of time for a task with a return "
			"raises a timeout exception.",
			[&]()
			{
				test_task.get( Milliseconds(100) );
			}
		);

		testForResult<int>(
			"A wait for a task with a return after a failed wait can succeed.",
			1, test_task.get()
		);
	}

	{
		auto test_task = makeTask(
			[&](int i)
			{
				std::this_thread::sleep_for(Milliseconds(500));
				return i;
			}
		);

		scheduler.addTask(test_task, 2);

		testForException<time_out>(
			"Waiting for an insufficient amount of time for a task with a return "
			"and argument raises a timeout exception.",
			[&]()
			{
				test_task.get( Milliseconds(100) );
			}
		);

		testForResult<int>(
			"A wait for a task with a return after a failed wait can succeed.",
			2, test_task.get()
		);
	}

	{
		auto test_task = makeTask(
			[&](int i)
			{
				throw invalid_argument("Task throwing Intentional exception for testing.");
				return i;
			}
		);

		scheduler.addTask(test_task, 1);

		testForException<invalid_argument>(
			"Task with return and argument that throws exception has that exception "
			"thrown on a wait for the task.",
			[&]()
			{
				test_task.get();
			}
		);

		testForResult(
			"Task that throws exxception is still considered finished.",
			TaskState::Finished, test_task.state()
		);
	}

	{
		Stopwatch stopwatch;
		uint32_t  timer_count = 0;
		
		auto tick_period = Milliseconds(100);
		bool timing_accurate = true;

		auto timer = makeTimer(
			[&]()
			{
				++timer_count;
				double actual_ms_trigger_time = Milliseconds(stopwatch.time()).count();
				double expected_ms            = timer_count * tick_period.count();

				if ( !approxEqual(actual_ms_trigger_time, expected_ms, 0.2f) )
					timing_accurate = false;
			}
		);

		timer.oneShot(tick_period);
		stopwatch.start();
		std::this_thread::sleep_for(Milliseconds(150));
		timer.stop();

		testForResult<bool>(
			"Timer: OneShot triggered after expected delay.",
			true, timing_accurate
		);
		
		testForResult<uint32_t>(
			"Timer: OneShot only triggered one time.",
			1, timer_count
		);

		timer_count = 0;
		timing_accurate = true;
		stopwatch.reset();

		timer.start(tick_period);
		stopwatch.start();
		std::this_thread::sleep_for(Milliseconds(350));
		timer.stop();

		testForResult<bool>(
			"Timer: Triggered at expected intervals.",
			true, timing_accurate
		);
		
		testForResult<uint32_t>(
			"Timer: Triggered expected number of times.",
			3, timer_count
		);
	}

	{
		PredicatedCondition condition;
		Stopwatch stopwatch;

		constexpr Milliseconds destroy_time(200.0f);
		constexpr Milliseconds timeout_time(500.0f);

		bool   result_destroyed = false;
		bool   result_timeout   = false;
		double ms_end_time      = 0.0;

		auto wait_task = makeTask(
			[&]()
			{
				try
				{
					condition.wait(
						[]
						{
							return false;
						},
						timeout_time
					);
				}
				catch (const object_destroyed&)
				{
					result_destroyed = true;
					ms_end_time = Milliseconds(stopwatch.time()).count();
				}
				catch (const time_out&)
				{
					result_timeout = true;
				}
			}
		);

		stopwatch.start();

		scheduler.addTask(wait_task);
		std::this_thread::sleep_for(destroy_time);
		condition.destroy();
		std::this_thread::sleep_for(Milliseconds(750));

		testForResult<bool>(
			"PredicatedCondition: A destroyed condition will have a destroyed result even if the object remains "
			"after the timeout.",
			true, (result_destroyed && !result_timeout)
		);

		testForResult<bool>(
			"PredicatedCondition: A destroyed condition returns from a wait call at a time reasonably close "
			"to the time of the destroy call.",
			true, approxEqual(destroy_time.count(), ms_end_time, 0.2f)
		);

		wait_task.wait();
	}

	{
		Stopwatch stopwatch;
		uint32_t  timer_count = 0;

		auto tick_period = Milliseconds(500);
		auto total_time = Milliseconds(2250);
		bool timing_accurate = true;

		Collections::Vector<double, 4> trigger_times;

		auto timer = makeTimer(
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
		auto one_shot_time = Milliseconds(500);

		{
			auto timer = makeTimer(
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
#if 0
	{
		PredicatedCondition condition_manager;
		
		std::array<bool, 4> conditions;
		conditions.fill(false);

		// 0 - no result
		// 1 - wait succeeded
		// 2 - object destroyed
		// 3 - wait timeout
		std::array<uint8_t, 5> task_results;
		task_results.fill(0);

		auto task_0 = makeTask(
			[&]()
			{
				auto precondition = [&]()
				{
					return conditions[0];
				};

				condition_manager.wait(
					precondition,
					[&]()
					{
						testForResult<bool>(
							"PredicatedCondition: task_0 had precondition met before action invoked.",
							true, precondition()
						);

						task_results[0] = 1;
					}
				);

				condition_manager.trigger(
					[&]()
					{
						conditions[1] = true;
					}
				);
			}
		);

		auto task_1 = makeTask(
			[&]()
			{
				auto precondition = [&]()
				{
					return conditions[1] && conditions[3];
				};

				condition_manager.wait(
					precondition,
					[&]()
					{
						testForResult<bool>(
							"PredicatedCondition: task_1 had precondition met before action invoked.",
							true, precondition()
						);

						task_results[1] = 1;
					}
				);

				condition_manager.trigger(
					[&]()
					{
						conditions[2] = true;
					}
				);
			}
		);

		auto task_2 = makeTask(
			[&]()
			{
				auto precondition = [&]()
				{
					return conditions[2];
				};

				condition_manager.wait(
					precondition,
					[&]()
					{
						testForResult<bool>(
							"PredicatedCondition: task_2 had precondition met before action invoked.",
							true, precondition()
						);

						task_results[2] = 1;
					}
				);
			}
		);

		auto task_3 = makeTask(
			[&]()
			{
				auto precondition = [&]()
				{
					return false;
				};

				try
				{
					condition_manager.wait(precondition);
				}
				catch ( const object_destroyed& )
				{
					task_results[3] = 2;
				}
			}
		);

		auto task_4 = makeTask(
			[&]()
			{
				auto start_time = system_clock::now();
				
				auto precondition = [&]()
				{
					return false;
				};

				try
				{
					condition_manager.wait(precondition, milliseconds(250));
				}
				catch ( const time_out& )
				{
					auto time_diff = std::chrono::duration_cast<milliseconds>(system_clock::now() - start_time);

					testForResult<bool>(
						"PredicatedCondition: Timeout exception thrown at expected delay.",
						true, relative_difference<uint64_t>(time_diff.count(), 250) < 0.1
					);
					
					task_results[4] = 3;
				}
			}
		);

		scheduler.addTask(task_0);
		scheduler.addTask(task_1);
		scheduler.addTask(task_2);
		scheduler.addTask(task_3);

		condition_manager.trigger(
			[&](){ conditions[0] = true; }
		);

		condition_manager.trigger(
			[&](){ conditions[3] = true; }
		);

		waitForAll({&task_0, &task_1, &task_2});

		scheduler.addTask(task_4);
		std::this_thread::sleep_for(milliseconds(500));

		condition_manager.destroy();

		waitForAll({&task_3, &task_4});

		testForResult<bool>(
			"PredicatedCondition: Preconditions met on expected tasks.",
			true, task_results[0] == 1 && task_results[1] == 1 && task_results[2] == 1
		);

		testForResult<bool>(
			"PredicatedCondition: Expected timout exception thrown.",
			true, task_results[4] == 3
		);

		testForResult<bool>(
			"PredicatedCondition: object_destroyed exception thrown for wait with unmet precondition.",
			true, task_results[3] == 2
		);
	}

	{
		PredicatedCondition condition_manager;
		uint32_t wake_count = 0;
		bool wake = false;

		int thread_wait_count = 0;

		auto count_main = [&]()
		{
			bool wait_count_added = false;
			
			try
			{
				condition_manager.wait(
					[&]()
					{
						if ( !wait_count_added )
						{
							wait_count_added = true;

							condition_manager.trigger(
								[&]()
								{
									++thread_wait_count;
								}
							);
						}
						
						return wake;
					},
					[&]()
					{
						++wake_count;
					}
				);
			}
			catch( const object_destroyed& )
			{
			};
		};

		auto task_0 = makeTask( count_main );
		auto task_1 = makeTask( count_main );
		auto task_2 = makeTask( count_main );
		auto task_3 = makeTask( count_main );

		scheduler.addTask(task_0);
		scheduler.addTask(task_1);
		scheduler.addTask(task_2);
		scheduler.addTask(task_3);

		condition_manager.wait(
			[&]()
			{
				return (thread_wait_count >= 4);
			}
		);

		condition_manager.trigger(
			[&]()
			{
				wake = true;
			}, 2
		);

		condition_manager.destroy();

		waitForAll({&task_0, &task_1, &task_2, &task_3});

		testForResult<bool>(
			"PredicatedCondition: Max wake count is honored.",
			true, wake_count == 2
		);
	}

	{
		PredicatedCondition condition_manager;
		bool start = false;
		bool wake_timed = false;
		bool timed_succeeded = false;

		auto first_task = makeTask(
			[&]()
			{
				condition_manager.wait(
					[&]()
					{
						if ( start )
						{
							wake_timed = true;
							Task::sleep( milliseconds(500) );

							return true;
						}

						return false;
					}
				);
			}
		);

		auto timed_task = makeTask(
			[&]()
			{
				condition_manager.wait(
					[&]()
					{
						return wake_timed;
					},
					[&]()
					{
						timed_succeeded = true;
					}
				);
			}
		);
	}

	{
		Producer<std::string> str_producer;
		Mutex output_lock;
		std::atomic<int> out_count(0);

		auto func1 = makeTask(
			[&]()
			{
				std::string out;
				while ( true )
				{
					try
					{
						str_producer.consume(out);
					
						MutexLocker lock(output_lock);
						std::cout << "Task 1: " << out << std::endl;
						++out_count;
					}
					catch (const time_out&)
					{
						return;
					}
					catch (const object_destroyed&)
					{
						return;
					}
				}
			}
		);

		FunctionTask func2(
			[&]()
			{
				std::string out;
				while ( true )
				{
					try
					{
						str_producer.consume(out);
					
						MutexLocker lock(output_lock);
						std::cout << "Task 2: " << out << std::endl;
						++out_count;
					}
					catch (const time_out&)
					{
						return;
					}
					catch (const object_destroyed&)
					{
						return;
					}
				}
			}
		);

		FunctionTask func3(
			[&]()
			{
				str_producer.push("One");
				str_producer.push("Two");
				str_producer.push("Three");
				str_producer.push("Four");
				str_producer.push("Five");
				str_producer.push("Six");
				str_producer.end();
			}
		);

		testByCheck(
			"Producer done when three threads have a wait complete. (Tests "
			"Producer, CallableTask, FunctionTask, and wait().)",
			[&]()
			{
				func1.runAsync();
				func2.runAsync();
				func3.runAsync();
				waitForAll( {&func1, &func2, &func3} );
			},
			[&]()
			{
				return (6 == out_count);
			}
		);
	}
#endif

	{
		// Timer timer;
		std::atomic<int> trigger_count(0);

		auto start_time = system_clock::now();

		Signals::Invokable<> timer_invoked;
		Condition timer_done;

		auto timer = makeTimer(
			[&]()
			{
				timer_invoked.invoke();
			}
		);

		Signals::FunctionEventHandler<> interval_handler(
			[&]()
			{
				++trigger_count;

				auto time_diff = system_clock::now() - start_time;
				auto time_diff_ms = (float)duration_cast<milliseconds>(time_diff).count();

				testForResult<bool>(
					"Timer firing approximately on one and half second intervals.",
					true, approxEqual(time_diff_ms, float(trigger_count) * 1500.0f, 0.05f)
				);

				if ( 3 == trigger_count )
				{
					timer_done.trigger();
				}
			}
		);

		interval_handler.bind(timer_invoked);
		timer.start(milliseconds(1500));
		timer_done.wait();
		timer.stop();

		auto end_time = system_clock::now();

		auto time_diff = end_time - start_time;
		auto time_diff_ms = duration_cast<milliseconds>(time_diff).count();

		testForResult<bool>(
			"Timer fired 3 times in 4.5 seconds.",
			true, approxEqual((float)time_diff_ms, 4500.0f, 0.01f)
		);

		interval_handler.unbind();
		timer_done.reset();

		Signals::FunctionEventHandler<> oneshot_handler(
			[&]()
			{
				auto time_diff = system_clock::now() - start_time;
				auto time_diff_ms = (float)duration_cast<milliseconds>(time_diff).count();

				testForResult<bool>(
					"One-shot fired at around 500ms",
					true, approxEqual(time_diff_ms, 500.0f, 0.05f)
				);

				timer_done.trigger();
			}
		);
		
		oneshot_handler.bind(timer_invoked);

		start_time = system_clock::now();

		timer.oneShot(500ms);
		timer_done.wait();
		end_time = system_clock::now();

		timer.stop();

		time_diff = end_time - start_time;
		time_diff_ms = duration_cast<milliseconds>(time_diff).count();

		testForResult<bool>(
			"One-shot ended after approximately 500ms",
			true, approxEqual((float)time_diff_ms, 500.0f, 0.1f)
		);
	}

	{
		Condition condition;

		testForResult<bool>(
			"Non-triggered condition returns false when called with a timout.",
			false, condition.wait(std::chrono::milliseconds(250))
		);
	}
#if 0
	{
		Condition condition;
		std::atomic<bool> wait_succeeded = false;

		FunctionTask wait_task(
			[&]()
			{
				wait_succeeded = condition.wait(std::chrono::seconds(2));
			}
		);

		FunctionTask trigger_task(
			[&]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				condition.trigger();
			}
		);

		scheduler.addTask(wait_task);
		scheduler.addTask(trigger_task);

		waitForAll({&wait_task, &trigger_task});

		testForResult<bool>(
			"Timed wait on condition succeeds when condition is triggered within timeframe.",
			true, wait_succeeded
		);
	}
#endif
	{
		int trigger_iterations = 0;
		Condition condition;
		
		auto now = std::chrono::steady_clock::now;
		bool pass_condition = false;

		auto checkPassTrue = [&]() -> bool
		{
			condition.reset();
			return pass_condition; 
		};

		auto start_time = now();

		testForResult<bool>(
			"Conditional wait fails after not seeing a trigger for a given time period.",
			false, conditionalTimedWait(condition, milliseconds(500), checkPassTrue)
		);

		auto end_time = steady_clock::now();

		testForResult<bool>(
			"Conditional wait took expected amount of time.",
			true, timeRelativeError(milliseconds(500), end_time - start_time) < 0.05
		);

		pass_condition = true;

		testForResult<bool>(
			"Conditional wait succeeds when test criteria is already met.",
			true, conditionalTimedWait(condition, milliseconds(500), checkPassTrue)
		);

		pass_condition = false;

		auto signal_task = makeTask(
			[&]()
			{
				std::this_thread::sleep_for(milliseconds(500));
				++trigger_iterations;
				condition.trigger();

				std::this_thread::sleep_for(milliseconds(500));
				++trigger_iterations;
				pass_condition = true;
				condition.trigger();

				std::this_thread::sleep_for(milliseconds(500));
				pass_condition = false;
				++trigger_iterations;
				condition.trigger();
			}
		);

		start_time = steady_clock::now();

		scheduler.addTask(signal_task);
		testForResult<bool>(
			"Conditional wait returns false when condition is triggered, "
			"but criteria is not met in time.",
			false, conditionalTimedWait(condition, milliseconds(900), checkPassTrue)
		);

		end_time = steady_clock::now();
		
		testForResult<bool>(
			"Conditional wait took expected amount of time.",
			true, timeRelativeError(milliseconds(900), end_time - start_time) < 0.05
		);

		signal_task.wait();
		trigger_iterations = 0;

		start_time = steady_clock::now();

		scheduler.addTask(signal_task);
		testForResult<bool>(
			"Conditional wait returns true when condition is triggered, "
			"but criteria is not met in time.",
			true, conditionalTimedWait(condition, milliseconds(1100), checkPassTrue)
		);

		end_time = steady_clock::now();
		
		testForResult<bool>(
			"Conditional wait took expected amount of time.",
			true, timeRelativeError(seconds(1), end_time - start_time) < 0.05
		);

		testForResult<bool>(
			"Conditional wait happened on expected iteration.",
			true, 2 == trigger_iterations
		);

		signal_task.wait();
	}
}
