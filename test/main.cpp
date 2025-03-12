extern void testConst();
extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testCompare();
extern void testCallableTraits();
extern void testCallable();
extern void testDefaultable();
extern void testMemory();
extern void testSignals();
extern void testNumber();
extern void testVec();
extern void testMatrix();
extern void testConcurrent();
extern void testSerialize();
extern void testString();
extern void testTemplateUtility();
extern void testTypeInfo();
extern void testUtility();
extern void testIpComm();
extern void testStreams();

int main()
{
	testCompare();
	testDefaultable();
	testIpComm();
	testTemplateUtility();
	testCallable();
	testConcurrent();
	testCallableTraits();
	testString();
	testStreams();
	testString();
	testMemory();
	testConst();
	testUtility();
	testTypeInfo();
	testCollections();
	testVec();
	testSerialize();
	testConcurrent();
	testMatrix();
	testNumber();
	testSignals();
	testInPlace();
	testAny();

	return 0;
}
