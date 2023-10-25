extern void testConst();
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
extern void testTypeInfo();
extern void testUtility();

#include <StdExt/Callable.h>

void run_callable(StdExt::CallableRef<int, int> func)
{
	func(1);
}

class TestCallable : public StdExt::Callable<int, int>
{
public:
	TestCallable() {}

protected:
	int run(int i) const override
	{
		return i + 1;
	}
};

int main()
{
	auto call = StdExt::Callable(
		[](int i)
		{
			return i + 1;
		}
	);

	run_callable(call);
	run_callable(TestCallable());

	run_callable(
		[](int i)
		{
			return i + 1;
		}
	);

	testConcurrent();
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
}
