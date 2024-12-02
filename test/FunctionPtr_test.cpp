#include <StdExt/FunctionPtr.h>

#include <StdExt/Test/Test.h>

#include <string>
#include <cmath>


class TestClass
{
private:
	int mValue{};
	std::string mName;

public:
	TestClass()
		: mValue(0)
	{
	}

	int addValue(int i) const
	{
		return mValue + i;
	}

	void setValue(int value)
	{
		mValue = value;
	}

	static int makeValue(int i, int j)
	{
		return i + j;
	}

	static TestClass makeTestClass(int value, std::string name)
	{
		TestClass ret;
		ret. mValue = value;
		ret.mName = name;

		return ret;
	}

	int ambiguous(float f_param, int i_param)
	{
		return 5 + mValue + std::roundf(f_param) + i_param;
	}

	float ambiguous(int i_param, float f_param)
	{
		return 2 + mValue + i_param + std::roundf(f_param);
	}
};

using namespace StdExt;
using namespace StdExt::Test;

void testFunctionPtr()
{
	TestClass TC;

	FunctionPtr<int(int)> f_ptr;
	f_ptr.bind<&TestClass::addValue>(&TC);

	f_ptr(1);

	FunctionPtr<int(int, int)> static_f_ptr;
	static_f_ptr.bind<&TestClass::makeValue>();

	static_f_ptr(1, 1);

#if 0
	using base_t = details::function_ptr_base<&TestClass::makeTestClass>;
	
	{
		constexpr auto static_ptr = FunctionPtr<&TestClass::makeTestClass>();
		constexpr auto member_ptr = FunctionPtr<&TestClass::setValue>();
		constexpr auto const_member_ptr = FunctionPtrOf<&TestClass::addValue>();

		auto result_1 = static_ptr(1, "string");
		member_ptr(&TC, 1);
		auto result_2 = const_member_ptr(&TC, 2);

		StaticFunction<&TestClass::makeTestClass> hard_func;
	}

	
	{
		constexpr StaticFunctionPtr static_ptr = &TestClass::makeTestClass;
		constexpr MemberFunctionPtr member_ptr = &TestClass::setValue;
		constexpr ConstMemberFunctionPtr const_member_ptr = &TestClass::addValue;

		static_ptr(1, "ret");
	}

	{
		constexpr MemberFunctionPtr<TestClass, float, int, float> amb_test_fif(&TestClass::ambiguous);
		constexpr MemberFunctionPtr<TestClass, int, float, int> amb_test_ifi(&TestClass::ambiguous);

		TestClass TC;
		TC.setValue(3);

		testForResult<float>(
			"Ambiguous function pointer correctly resolves using argument types. <float (int, float)>",
			10.0f, amb_test_fif(&TC, 3, 2.0f)
		);

		testForResult<int>(
			"Ambiguous function pointer correctly resolves using argument types. <int (float, int)>",
			13, amb_test_ifi(&TC, 3.0f, 2)
		);
	}
#endif
}