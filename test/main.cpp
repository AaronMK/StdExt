extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testMemory();
extern void testSignals();
extern void testFunctionPtr();
extern void testNumber();
extern void testVec();
extern void testMatrix();
extern void testConcurrent();

#include <StdExt/Concurrent/Producer.h>

int main()
{
	testConcurrent();
	testVec();
	testMatrix();
	testNumber();
	testFunctionPtr();
	testSignals();
	testMemory();
	testCollections();
	testInPlace();
	testAny();
}