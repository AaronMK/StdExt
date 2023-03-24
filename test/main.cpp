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

int main()
{
	testConcurrent();
	testMemory();
	testUtility();
	testUnicode();
	testTypeInfo();
	testCollections();
	testVec();
	testSerialize();
	testString();
	testMatrix();
	testNumber();
	testSignals();
	testInPlace();
	testAny();
}