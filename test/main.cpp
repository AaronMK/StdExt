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

int main()
{
	using namespace StdExt;
	using namespace StdExt::Concurrent;

	auto testFunc = std::function(
		[](int i)
		{
			return i + 1;
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