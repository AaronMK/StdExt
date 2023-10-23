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
extern void testTypeInfo();
extern void testUtility();

#include <StdExt/Callable.h>

int main()
{
	auto call = StdExt::makeCallable(
		[](int i)
		{
			return i + 1;
		}
	);

	auto result = call(1);

	testConcurrent();
	testString();
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
