#include <StdExt/Concurrent/FunctionTask.h>
#include <StdExt/Concurrent/MessageLoop.h>
#include <StdExt/Concurrent/Producer.h>
#include <StdExt/Concurrent/Mutex.h>
#include <StdExt/Concurrent/Timer.h>

#include <StdExt/Signals/FunctionHandlers.h>

#include <StdExt/Test/Test.h>

#include <array>
#include <string>
#include <iostream>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Concurrent;

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
				CompleteFlags[0] = true;
			}
		);

		subtask(
			[this]()
			{
				CompleteFlags[1] = true;
			}
		);
	}
};

class StringLoop : public MessageLoop<std::string>
{
public:
	StringLoop()
	{

	}

	virtual void handleMessage(const std::string& message)
	{
		std::cout << "Loop task: " << message << std::endl;
	}

protected:
	
};

void testConcurrent()
{
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

		FunctionTask func1(
			[&]()
			{
				std::string out;
				while ( str_producer.consume(out) )
				{
					MutexLocker lock(output_lock);
					std::cout << "Task 1: " << out << std::endl;
					++out_count;
				}
			}
		);

		FunctionTask func2(
			[&]()
			{
				std::string out;
				while ( str_producer.consume(out) )
				{
					MutexLocker lock(output_lock);
					std::cout << "Task 2: " << out << std::endl;
					++out_count;
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
			"Producer done when three threads have a wait complete.  (Tests Queue, "
			"Producer, RWLock, FunctionTask, and wait().)",
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
		FunctionHandlerLoop<std::string> strLoop(
			[&](const std::string& message)
			{
				std::cout << "Function Loop task: " << message << std::endl;
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
		FunctionHandlerLoop<const char*> strLoop(
			[&](const char* message)
			{
				std::cout << "Function Loop pointer task: " << *message << std::endl;
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
		Condition done;

		auto start_time = std::chrono::system_clock::now();

		Signals::FunctionEventHandler<> timer_handler(
			[&]()
			{
				std::cout << "Timer Triggered" << std::endl;
				++trigger_count;

				auto time_diff = std::chrono::system_clock::now() - start_time;
				auto time_diff_ms = (float)std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();

				testForResult<bool>(
					"Timer firing approximately on one second intervals.",
					true, approxEqual(time_diff_ms, float(trigger_count) * 1000.0f, 0.05f)
				);

				if ( 3 == trigger_count )
				{
					timer.stop();
					done.trigger();
				}
			}
		);

		timer_handler.bind(timer);
		timer.start(std::chrono::seconds(1));
		done.wait();

		auto end_time = std::chrono::system_clock::now();

		auto time_diff = end_time - start_time;
		auto time_diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_diff).count();

		testForResult<bool>(
			"Timer fired 3 times in 3 seconds.",
			true, approxEqual((float)time_diff_ms, 3000.0f, 0.01f)
		);
	}
}