#include <StdExt/FunctionPtr.h>
#include <StdExt/Test/Test.h>

#include <string>
#include <cmath>
#include <functional>

class BaseClass
{
protected:
	int mValue;

public:
	constexpr BaseClass()
	{
		mValue = 0;
	}

	int nonVirtualFunc(int i_val, float f_val)
	{
		mValue = 1 + i_val + f_val;
		return mValue;
	}

	virtual int virtualFunc(int i_val, float f_val)
	{
		mValue = 1 + i_val + f_val;
		return mValue;
	}

	virtual int pureVirtualFunc(int i_val, float f_val) = 0;

	int getValue() const noexcept
	{
		return mValue;
	}
};

class DerivedClass : public BaseClass
{
public:
	constexpr DerivedClass()
		: BaseClass()
	{
	}

	int nonVirtualFunc(int i_val, float f_val)
	{
		mValue = 2 + i_val + f_val;
		return mValue;
	}

	int virtualFunc(int i_val, float f_val) override
	{
		mValue = 2 + i_val + f_val;
		return mValue;
	}

	int pureVirtualFunc(int i_val, float f_val) override
	{
		mValue = 3 + i_val + f_val;
		return mValue;
	}


};

class TestClass
{
private:
	int mValue{};

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

template<CallableWith<void, int> auto... calls>
class TemplatedCall
{
public:
	static void callAll(int arg)
	{
		(calls(arg), ...);
	}
};

void testFunctionPtr()
{
	DerivedClass derived_object;
	BaseClass* base_pointer = &derived_object;

	{
		const auto base_virtual_call = bind<&BaseClass::virtualFunc>(base_pointer);
		const auto base_pure_virtual_call = bind<&BaseClass::pureVirtualFunc>(base_pointer);
		const auto derived_call_to_base_func = bind<&DerivedClass::getValue>(&derived_object);
		const auto base_non_virtual_call = bind<&BaseClass::nonVirtualFunc>(&derived_object);
		const auto derived_non_virtual_call = bind<&DerivedClass::nonVirtualFunc>(&derived_object);

		Test::testForResult(
			"Templated Bind - Bind by a base function pointer to derived object calls derived override.",
			5, base_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Templated Bind - Bind by a base pure virtual function pointer to derived object calls derived override.",
			6, base_pure_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Templated Bind - Non-virtual call invokes base according to the function pointer.",
			4, base_non_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Templated Bind - Non-virtual call invokes derived according to the function pointer.",
			5, derived_non_virtual_call(1.0f, 2.0f)
		);
	}

	{
		const auto base_virtual_call = bind(&BaseClass::virtualFunc, base_pointer);
		const auto base_pure_virtual_call = bind(&BaseClass::pureVirtualFunc, base_pointer);
		const auto base_non_virtual_call = bind(&BaseClass::nonVirtualFunc, &derived_object);
		const auto derived_non_virtual_call = bind(&DerivedClass::nonVirtualFunc, &derived_object);

		Test::testForResult(
			"Function Pointer Bind - Bind by a base function pointer to derived object calls derived override.",
			5, base_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Function Pointer Bind - Bind by a base pure virtual function pointer to derived object calls derived override.",
			6, base_pure_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Function Pointer Bind - Non-virtual call invokes base according to the function pointer.",
			4, base_non_virtual_call(1.0f, 2.0f)
		);

		Test::testForResult(
			"Function Pointer Bind - Non-virtual call invokes derived according to the function pointer.",
			5, derived_non_virtual_call(1.0f, 2.0f)
		);
	}

	static constexpr TestClass TC;

	constexpr auto tc_labmda = [](int i) constexpr
	{
		return i + 2;
	};

	auto binded_lamda = CallablePtr<float(float)>(&tc_labmda);
	constexpr auto binded_obj = bind<&TestClass::addValue>(&TC);

	auto lambda_result = binded_lamda(2.0f);
	auto binded_result = binded_obj(2.0f);

	constexpr auto auto_bound = bind(&TestClass::addValue, &TC);
	constexpr auto template_bound = bind<&TestClass::addValue>(&TC);

	constexpr auto bound_result = auto_bound(1.0f);

	constexpr auto static_bound = bind(&TestClass::makeTestClass);

	constexpr auto size_test = sizeof(auto_bound);
	constexpr auto template_size_test = sizeof(template_bound);
	constexpr auto static_size_test = sizeof(static_bound);

	constexpr bool is_false_callable_with = CallableWith<decltype(auto_bound), std::string, std::string>;

	CallablePtr<float(int)> ptr_to_templated(&template_bound);

	TemplatedCall<
		bind<&TestClass::addValue>(&TC),
		[](int i)
		{
			return i + 3;
		}
	> all_call;

	all_call.callAll(3);

#if 0

	CallablePtr<int(int)> f_ptr;
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