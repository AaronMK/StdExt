#include <StdExt/Concurrent/FunctionTask.h>

#include <StdExt/Test/Test.h>

#include <array>

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
}