extern void testConst();
extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testCallable();
extern void testDefaultable();
extern void testFunctionTraits();
extern void testMemory();
extern void testSignals();
extern void testNumber();
extern void testVec();
extern void testMatrix();
extern void testSerialize();
extern void testString();
extern void testTemplateUtility();
extern void testTypeInfo();
extern void testUtility();
extern void testStreams();
extern void testCallable();

int main()
{
	testFunctionTraits();
	testCallable();
	testDefaultable();
	testTemplateUtility();
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
	testMatrix();
	testNumber();
	testSignals();
	testInPlace();
	testAny();

	return 0;
}
