#include <StdExt/Concurrent/FunctionTask.h>
#include <StdExt/Concurrent/Producer.h>
#include <StdExt/Concurrent/Mutex.h>

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

		FunctionTask func1(
			[&]()
			{
				std::string out;
				while ( str_producer.consume(out) )
				{
					MutexLocker lock(output_lock);
					std::cout << "Task 1: " << out << std::endl;
				}
			}
		);
		func1.runAsync();

		FunctionTask func2(
			[&]()
			{
				std::string out;
				while ( str_producer.consume(out) )
				{
					MutexLocker lock(output_lock);
					std::cout << "Task 2: " << out << std::endl;
				}
			}
		);
		func2.runAsync();

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
		func3.runAsync();

		waitForAll( {&func1, &func2, &func3} );
	}
}