#include <StdExt/Concurrent/PredicatedCondition.h>
#include <StdExt/Concurrent/CallableTask.h>
#include <StdExt/Concurrent/FunctionTask.h>
#include <StdExt/Concurrent/MessageLoop.h>
#include <StdExt/Concurrent/Producer.h>
#include <StdExt/Concurrent/TaskLoop.h>
#include <StdExt/Concurrent/Mutex.h>
#include <StdExt/Concurrent/Timer.h>
#include <StdExt/Concurrent/Wait.h>

#include <StdExt/Signals/FunctionHandlers.h>

#include <StdExt/Test/Test.h>

#include <array>
#include <chrono>
#include <string>
#include <thread>
#include <iostream>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Concurrent;

using namespace std;
using namespace std::chrono;

class SubtaskTest : public Task
{
public:
	std::array<int, 2> CompleteFlags;

	SubtaskTest()
	{
		CompleteFlags = {false, false};
	}

	virtual void run() override
	{
		subtask(
			[this]()
			{
				this_thread::sleep_for(milliseconds(250));
				CompleteFlags[0] = true;
			}
		);

		subtask(
			[this]()
			{
				this_thread::sleep_for(milliseconds(250));
				CompleteFlags[1] = true;
			}
		);
	}

	bool isComplete() const
	{
		return (!isRunning() && CompleteFlags[0] && CompleteFlags[1]);
	}

	void reset()
	{
		CompleteFlags.fill(false);
	}
};

class StringLoop : public MessageLoop<std::string>
{
public:
	StringLoop()
	{
	}

protected:
	virtual void handleMessage(handler_param_t message)
	{
		std::cout << "Loop task: " << message << std::endl;
	}
};

void testConcurrent()
{
	auto timeRelativeError = [](nanoseconds expected, nanoseconds observed)
	{
		return relative_difference(expected.count(), observed.count());
	};

	{
		std::array<bool, 4> conditions;
		conditions.fill(false);

		PredicatedCondition condition_manager;

		auto task_0 = makeTask(
			[&]()
			{
				auto precondition = [&]()
				{
					return conditions[0];
				};

				condition_manager.wait(precondition);
				condition_manager.protectedAction(
					[&]()
					{
						testForResult<bool>(
							"PredicatedCondition: task_0 had precondition met before action invoked.",
							true, precondition()
						);
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

				condition_manager.wait(precondition);
				condition_manager.protectedAction(
					[&]()
					{
						testForResult<bool>(
							"PredicatedCondition: task_1 had precondition met before action invoked.",
							true, precondition()
						);
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

				condition_manager.wait(precondition);
				condition_manager.protectedAction(
					[&]()
					{
						testForResult<bool>(
							"PredicatedCondition: task_2 had precondition met before action invoked.",
							true, precondition()
						);
					}
				);
			}
		);

		task_0.runAsync();
		task_1.runAsync();
		task_2.runAsync();

		condition_manager.trigger(
			[&](){ conditions[0] = true; }
		);

		condition_manager.trigger(
			[&](){ conditions[3] = true; }
		);

		waitForAll({&task_0, &task_1, &task_2});
	}

	{
		SubtaskTest test;

		testByCheck(
			"Subtasks completed before main task is considered complete.",
			[&]()
			{
				test.runAsync();
			},
			[&]() -> bool
			{
				test.wait();
				return ( test.CompleteFlags[0] && test.CompleteFlags[1] );
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

	{
		StringLoop strLoop;
		strLoop.runAsync();
		
		strLoop.push("One");
		strLoop.push("Two");
		strLoop.push("Three");

		strLoop.barrier();

		strLoop.push("Four");
		strLoop.push("Five");
		strLoop.push("Six");
		
		strLoop.end();
		strLoop.wait();
	}

	{
		std::string aggregate;
		
		FunctionHandlerLoop<std::string> strLoop(
			[&](std::string& message)
			{
				aggregate += message;
			}
		);

		strLoop.runAsync();
		
		strLoop.push("One");
		strLoop.push("Two");
		strLoop.push("Three");

		strLoop.barrier();

		strLoop.push("Four");
		strLoop.push("Five");
		strLoop.push("Six");
		
		strLoop.end();
		strLoop.wait();

		testForResult<std::string>(
			"Message loop runs in insertion order.",
			"OneTwoThreeFourFiveSix", aggregate
		);
	}

	{
		FunctionHandlerLoop<std::string&> strLoop(
			[&](std::string& message)
			{
				std::string moved(std::move(message));
				std::cout << "Function Loop task: " << moved << std::endl;
			}
		);

		strLoop.runAsync();

		std::array<std::string, 3> strings;
		strings[0] = "One";
		strings[1] = "two";
		strings[2] = "three";

		strLoop.push(strings[0]);
		strLoop.push(strings[1]);
		strLoop.push(strings[2]);

		strLoop.end();
		strLoop.wait();
	}

	{
		FunctionHandlerLoop<const char*> strLoop(
			[&](const char* message)
			{
				std::cout << "Function Loop pointer task: " << std::string(message) << std::endl;
			}
		);

		strLoop.runAsync();
		
		strLoop.push("One");
		strLoop.push("Two");
		strLoop.push("Three");

		strLoop.barrier();

		strLoop.push("Four");
		strLoop.push("Five");
		strLoop.push("Six");
		
		strLoop.end();
		strLoop.wait();
	}

	{
		Timer timer;
		std::atomic<int> trigger_count(0);

		auto start_time = system_clock::now();

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
					timer.stop();
			}
		);

		interval_handler.bind(timer);
		timer.start(milliseconds(1500));
		timer.wait();

		auto end_time = system_clock::now();

		auto time_diff = end_time - start_time;
		auto time_diff_ms = duration_cast<milliseconds>(time_diff).count();

		testForResult<bool>(
			"Timer fired 3 times in 4.5 seconds.",
			true, approxEqual((float)time_diff_ms, 4500.0f, 0.01f)
		);

		interval_handler.unbind();


		Signals::FunctionEventHandler<> oneshot_handler(
			[&]()
			{
				auto time_diff = system_clock::now() - start_time;
				auto time_diff_ms = (float)duration_cast<milliseconds>(time_diff).count();

				testForResult<bool>(
					"One-shot fired at around 500ms",
					true, approxEqual(time_diff_ms, 500.0f, 0.05f)
				);
			}
		);
		
		oneshot_handler.bind(timer);

		start_time = system_clock::now();

		timer.oneShot(500ms);
		timer.wait();

		end_time = system_clock::now();

		time_diff = end_time - start_time;
		time_diff_ms = duration_cast<milliseconds>(time_diff).count();

		testForResult<bool>(
			"One-shot ended after approximately 500ms",
			true, approxEqual((float)time_diff_ms, 500.0f, 0.1f)
		);
	}

	{
		SubtaskTest sub_test1;
		SubtaskTest sub_test2;

		FunctionTask seq_check_task(
			[&]()
			{
				testForResult<bool>(
					"Task and sub-tasks complete before TaskLoop calls another task.",
					true, sub_test1.isComplete()
				);
			}
		);
		
		TaskLoop loop;

		{
			loop.add(&sub_test1);

			testForException<invalid_operation>(
				"Exception is thrown if trying to inline TaskLoop if end() is not called first.",
				[&]() { loop.runInline(); }
			);

			testForException<invalid_operation>(
				"Exception is thrown if trying to add a running task to TaskLoop.",
				[&]() { loop.add(&sub_test1); }
			);

			loop.add(&seq_check_task);
			loop.add(&sub_test2);

			loop.runAsync();

			loop.end();

			testForException<invalid_operation>(
				"Exception is thrown if trying to add a task to TaskLoop after end() is called.",
				[&]() { loop.add(&sub_test1); }
			);

			loop.wait();

			testForResult<bool>(
				"All added tasks have completed when TaskLoop ends.",
				true, sub_test2.isComplete()
			);
		}

		{
			sub_test1.reset();
			sub_test2.reset();

			loop.add(&sub_test1);
			loop.add(&seq_check_task);
			loop.add(&sub_test2);
			loop.end();

			loop.runInline();
			loop.wait();

			testForResult<bool>(
				"TaskLoop can run inline after end() is called and has finished when the call returns.",
				false, loop.isRunning()
			);

			testForResult<bool>(
				"All added tasks have completed when TaskLoop ends an inline run.",
				true, sub_test2.isComplete()
			);
		}

		{
			sub_test1.reset();
			sub_test2.reset();

			loop.add(&sub_test1);
			loop.add(&seq_check_task);
			loop.add(&sub_test2);

			loop.runAsync();

			sub_test2.wait();

			testForResult<bool>(
				"Task in TaskLoop can be waited on and completes.",
				true, sub_test2.isComplete()
			);

			sub_test2.reset();

			loop.add(&sub_test2);
			sub_test2.wait();

			loop.end();

			testForResult<bool>(
				"Task can be re-added to TaskLoop.",
				true, sub_test2.isComplete()
			);

			loop.wait();
		}
	}

	{
		Condition condition;

		testForResult<bool>(
			"Non-triggered condition returns false when called with a timout.",
			false, condition.wait(std::chrono::milliseconds(250))
		);
	}

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

		wait_task.runAsync();
		trigger_task.runAsync();

		waitForAll({&wait_task, &trigger_task});

		testForResult<bool>(
			"Timed wait on condition succeeds when condition is triggered within timeframe.",
			true, wait_succeeded
		);
	}

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

		FunctionTask signal_task(
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

		signal_task.runAsync();
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

		signal_task.runAsync();
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