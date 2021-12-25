#include <StdExt/Test/Test.h>
#include <StdExt/FunctionPtr.h>

#include "TestClasses.h"

using namespace StdExt;
using namespace StdExt::Test;

static int test_value = 18950675;

static int getTestValue()
{
	return test_value;
}

static int addToTestValue(int val)
{
	return test_value + val;
}

static void setTestValue(int next_val)
{
	test_value = next_val;
}

void testFunctionPtr()
{
	{
		FunctionPtr<int> test_ptr(&getTestValue);

		testForResult(
			"Static no-arg function pointer properly calls.",
			test_value, test_ptr()
		);
	}

	{
		FunctionPtr<int, int> test_ptr(&addToTestValue);

		testForResult(
			"Static function pointer with argument(s) properly calls.",
			test_value + 3, test_ptr(3)
		);
	}

	{
		FunctionPtr<int> test_ptr([]() { return addToTestValue(1); });

		testForResult(
			"Static function pointer defined as lambda properly calls.",
			test_value + 1, test_ptr()
		);
	}

	{
		int next_test_val = 823458;
		testByCheck(
			"Static void return function properly calls.",
			[&]()
			{
				FunctionPtr<void, int> test_ptr(&setTestValue);
				test_ptr(next_test_val);
			},
			[&]()
			{
				return (next_test_val == getTestValue());
			}
		);
	}

	{
		TestBase test_obj;

		FunctionPtr<size_t> test_ptr(&TestBase::id, &test_obj);

		testForResult(
			"Pointer to non-static member function properly calls.",
			test_obj.id(), test_ptr()
		);
	}

	{
		TestNoCopyMove test_obj;
		FunctionPtr<std::type_index> test_ptr(&TestBase::typeIndex, &test_obj);

		testForResult(
			"Pointer to base class function properly calls sub-class override.",
			Type<TestNoCopyMove>::index(), test_ptr()
		);
	}
}