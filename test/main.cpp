extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testMemory();
extern void testSignals();
extern void testFunctionPtr();
extern void testNumber();

int main()
{
	testNumber();
	testFunctionPtr();
	testSignals();
	testMemory();
	testCollections();
	testInPlace();
	testAny();
}