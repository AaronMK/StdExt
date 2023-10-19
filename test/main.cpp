extern void testConst();
extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testMemory();
extern void testSignals();
extern void testNumber();
extern void testVec();
extern void testMatrix();
extern void testConcurrent();
extern void testSerialize();
extern void testString();
extern void testUnicode();
extern void testTypeInfo();
extern void testUtility();

#include <StdExt/Concurrent/CallableTask.h>
#include <StdExt/Concurrent/Scheduler.h>
#include <iostream>

using namespace StdExt;
using namespace StdExt::Concurrent;

class TestTask : Task<int, int, int>
{
public:
	TestTask()
	{
	}

protected:
	int run(int a, int b) override
	{
		return a + b;
	}
};

class TestTaskVoid : Task<void, int, int>
{
public:
	TestTaskVoid()
	{
	}

protected:
	void run(int a, int b) override
	{
		std::cout << a + b << std::endl;
	}
};

class TestTaskVoidRet : Task<int>
{
public:
	TestTaskVoidRet()
	{
	}

protected:
	int run() override
	{
		return 5;
	}
};

class TestTaskVoidVoid : Task<void>
{
public:
	TestTaskVoidVoid()
	{
	}

protected:
	void run() override
	{
		std::cout << 5 << std::endl;
	}
};

int main()
{
	Scheduler main_scheduler(u8"main()");
	
	int i = 0;

	auto testFunc = makeTask(
		[&](int param)
		{
			i = param;
		}
	);

	main_scheduler.addTask(testFunc, 1);

	testConcurrent();
	testString();
	testUnicode();
	testMemory();
	testConst();
	testUtility();
	testTypeInfo();
	testCollections();
	testVec();
	testSerialize();
	testMatrix();
	testNumber();
	testSignals();
	testInPlace();
	testAny();
}