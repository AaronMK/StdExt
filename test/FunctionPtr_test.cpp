#include <StdExt/FunctionPtr.h>
#include <StdExt/Test/Test.h>

#include <string>
#include <cmath>
#include <functional>

class TestClass
{
private:
	int mValue{};
	std::string mName;

public:
	constexpr TestClass()
		: mValue(0)
	{
	}

	constexpr int addValue(int i) const
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

	static int StaticNoExcept(int i) noexcept
	{
		return i + 2;
	}

	static int StaticExcept(int i)
	{
		if (i < 0)
			throw std::invalid_argument("Argument must be positive.");

		return i + 2;
	}

	static TestClass makeTestClass(int value, std::string name)
	{
		TestClass ret;
		ret.mValue = value;
		ret.mName = name;

		return ret;
	}

	static int ambiguous(float f_param, int i_param)
	{
		return 5 + std::roundf(f_param) + i_param;
	}

	float ambiguous(int i_param, float f_param) const
	{
		return 2 + mValue + i_param + std::roundf(f_param);
	}

	int ambiguous(int i_param, float f_param)
	{
		return 2 + mValue + i_param + std::roundf(f_param);
	}
};

using namespace StdExt;
using namespace StdExt::Test;

void testFunctionPtr()
{
#if 0
	constexpr auto val = TestFuncTraitsClass<&TestClass::StaticNoExcept>::inherited_is_noexcept;

	constexpr FunctionPointerTraits NoExceptFT(&TestClass::StaticNoExcept);
	static_assert(NoExceptFT.is_const);
	static_assert(false == NoExceptFT.is_member);
	static_assert(NoExceptFT.is_noexcept);

	constexpr FunctionPointerTraits ExceptFT(&TestClass::StaticExcept);
	static_assert(ExceptFT.is_const);
	static_assert(false == ExceptFT.is_member);
	static_assert(false == ExceptFT.is_noexcept);

	Detail::Selector<float, int> select;
	constexpr auto f_traits = select(&TestClass::ambiguous);

	const TestClass TC;
	auto float_bind = bind<&TestClass::addValue, int, int>(&TC);
#endif

	static constexpr TestClass TC;

	constexpr auto tc_labmda = [](int i) constexpr
	{
		return i + 2;
	};

	auto binded_labmda = bind<int, float>(&tc_labmda);
	auto binded_obj = bind<&TestClass::addValue, int, float>(&TC);

	auto lambda_result = binded_labmda(2.0f);
	auto binded_result = binded_obj(2.0f);

	constexpr BoundFunctionPointer auto_bound(&TestClass::addValue, &TC);
	constexpr auto bound_result = auto_bound(1.0f);
	

	constexpr BoundFunctionPointer static_bound(&TestClass::makeTestClass);

	constexpr auto size_test = sizeof(auto_bound);
	constexpr auto static_size_test = sizeof(static_bound);

#if 0

	FunctionPtr<int(int)> f_ptr;
	f_ptr.bind<&TestClass::addValue>(&TC);

	f_ptr(1);

	FunctionPtr<int(int, int)> static_f_ptr;
	static_f_ptr.bind<&TestClass::makeValue>();
	static_f_ptr(1, 1);

	static constinit std::string ci_string("constinit string");
	constexpr std::string* str_addr = &ci_string;


	FunctionPtr<int(float, int)> amb_a_ptr;

	constexpr auto overload_resolve = getOverload<int(int, float)>::try_cast(&TestClass::ambiguous);
	FunctionPtr<int(int, float)> resolved_bind;
	resolved_bind.bind<overload_resolve>(&TC);
	using base_t = Detail::function_ptr_base<&TestClass::makeTestClass>;
	
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