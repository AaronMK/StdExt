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

int main()
{
	int i = 0;

	auto testFunc = makeTask(
		[&](int param)
		{
			i = param;
		}
	);

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