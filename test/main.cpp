extern void testConst();
extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testCallableTraits();
extern void testCallable();
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

int main()
{
	testCallable();
	testConcurrent();
	testCallableTraits();
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

	return 0;
}
