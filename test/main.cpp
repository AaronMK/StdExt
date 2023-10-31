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

	auto lambda_plus_one = [](int i)
		{
			return i + 1;
		};

	auto lambda_plus_two = [](int i)
		{
			return i + 2;
		};

	StdExt::CallableRef<int, int> call_ref;

	call_ref = lambda_plus_one;
	int result = call_ref(1);

	call_ref = lambda_plus_two;
	result = call_ref(1);

	run_callable(call);
	result = call_ref(1);

	run_callable(
		[](int i)
		{
			return i + 1;
		}
	);

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
}
