// #include <StdExt/InPlace.h>

extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testMemory();
extern void testSignals();

int main()
{
	testMemory();
	testCollections();
	testInPlace();
	testAny();
}