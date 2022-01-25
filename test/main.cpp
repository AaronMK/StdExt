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
extern void testSerialize();
extern void testString();
extern void testUnicode();

int main()
{
	testCollections();
	testVec();
	testSerialize();
	testUnicode();
	testString();
	testConcurrent();
	testMatrix();
	testNumber();
	testFunctionPtr();
	testSignals();
	testMemory();
	testInPlace();
	testAny();
}