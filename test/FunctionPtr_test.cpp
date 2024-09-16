#include <StdExt/FunctionPtr.h>

#include <string>

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

	static TestClass makeTestClass(int value, const std::string& name)
	{
		return TestClass();
	}
};

using namespace StdExt;

void testFunctionPtr()
{
	TestClass TC;
	
	{
		constexpr auto static_ptr = StaticFunctionPtr(&TestClass::makeTestClass);
		constexpr auto member_ptr = MemberFunctionPtr(&TestClass::setValue);
		constexpr auto const_member_ptr = ConstMemberFunctionPtr(&TestClass::addValue);

		auto result_1 = static_ptr(1, "string");
		member_ptr(&TC, 1);
		auto result_2 = const_member_ptr(&TC, 2);
	}

	
	{
		constexpr StaticFunctionPtr static_ptr = &TestClass::makeTestClass;
		constexpr MemberFunctionPtr member_ptr = &TestClass::setValue;
		constexpr ConstMemberFunctionPtr const_member_ptr = &TestClass::addValue;
	}
}