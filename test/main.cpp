extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testMemory();
extern void testSignals();
extern void testFunctionPtr();
extern void testNumber();
extern void testVec();
extern void testMatrix();

int main()
{
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