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
extern void testIpComm();

int main()
{
	testString();
	testIpComm();
	testUtility();
	testUnicode();
	testTypeInfo();
	testCollections();
	testVec();
	testSerialize();
	testConcurrent();
	testMatrix();
	testNumber();
	testSignals();
	testMemory();
	testInPlace();
	testAny();
}