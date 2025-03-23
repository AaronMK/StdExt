#include <StdExt/CallableTraits.h>
#include <StdExt/Number.h>

#include <functional>
#include <string>
#include <tuple>

using namespace StdExt;

class TestClass
{
public:
	void operator()(int a)
	{
	}
	
	static std::string makeString(const char* str)
	{
		return "";
	}

	float makeFloat(int i, float j)
	{
		return Number::convert<float>(i + j);
	}

	virtual int opFloats(float left, float right) = 0;

	int constFunc(int i) const
	{
		return i + 2;
	}
};

class TestDerived : public TestClass
{
public:
	int opFloats(float left, float right) override
	{
		return static_cast<int>(left * right);
	}
};

static std::string makeIntString(int i)
{
	return std::to_string(i);
}


static int foo(const std::string& param_a, int param_b)
{
	return 1;
}

class Bar
{
public:
	static int func_static(const std::string& param_a, int param_b)
	{
		return 0;
	}

	int func_a(const std::string& param_a, int param_b)
	{
		return 0;
	}

	int func_b(const std::string& param_a, int param_b) const
	{
		return 0;
	}
};

void testCallableTraits()
{
	static_assert(std::same_as<FunctionTraits<&foo>::return_t, int>);
	static_assert(std::same_as<FunctionTraits<&Bar::func_static>::return_t, int>);
	static_assert(std::same_as<FunctionTraits<&Bar::func_a>::return_t, int>);
	static_assert(std::same_as<FunctionTraits<&Bar::func_b>::return_t, int>);

	static_assert(std::same_as<
		FunctionTraits<&foo>::params_t, Types<const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_static>::params_t,
		Types<const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_a>::params_t,
		Types<const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_b>::params_t,
		Types<const std::string&, int>
	>);

	static_assert(FunctionTraits<&foo>::arg_count == 2);
	static_assert(FunctionTraits<&Bar::func_static>::arg_count == 2);
	static_assert(FunctionTraits<&Bar::func_a>::arg_count == 2);
	static_assert(FunctionTraits<&Bar::func_b>::arg_count == 2);

	static_assert(!FunctionTraits<&foo>::is_member);
	static_assert(!FunctionTraits<&Bar::func_static>::is_member);
	static_assert( FunctionTraits<&Bar::func_a>::is_member);
	static_assert( FunctionTraits<&Bar::func_b>::is_member);

	static_assert( FunctionTraits<&foo>::is_const);
	static_assert( FunctionTraits<&Bar::func_static>::is_const);
	static_assert(!FunctionTraits<&Bar::func_a>::is_const);
	static_assert( FunctionTraits<&Bar::func_b>::is_const);

	static_assert(std::same_as< FunctionTraits<&foo>::nth_arg_t<0>, const std::string&>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_static>::nth_arg_t<0>, const std::string&>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_a>::nth_arg_t<0>, const std::string&>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_b>::nth_arg_t<0>, const std::string&>);

	static_assert(std::same_as< FunctionTraits<&foo>::nth_arg_t<1>, int>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_static>::nth_arg_t<1>, int>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_a>::nth_arg_t<1>, int>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_b>::nth_arg_t<1>, int>);

	static_assert(std::same_as<
		FunctionTraits<&foo>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_static>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_a>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_b>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);

	static_assert(std::same_as<
		FunctionTraits<&foo>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_static>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_a>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_b>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);

	static_assert(std::same_as<
		FunctionTraits<&foo>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_static>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_a>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);
	static_assert(std::same_as<
		FunctionTraits<&Bar::func_b>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);

	static_assert(std::same_as< FunctionTraits<&foo>::class_type, void>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_static>::class_type, void>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_a>::class_type, Bar>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_b>::class_type, Bar>);

	static_assert(std::same_as< FunctionTraits<&foo>::target_type, void>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_static>::target_type, void>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_a>::target_type, Bar*>);
	static_assert(std::same_as< FunctionTraits<&Bar::func_b>::target_type, const Bar*>);

	static_assert(Callable<int>         == false);
	static_assert(Callable<std::string> == false);
	static_assert(Callable<TestClass>   == true);

	static_assert(FunctionPointer<int>                             == false);
	static_assert(FunctionPointer<std::string>                     == false);
	static_assert(FunctionPointer<TestClass>                       == false);
	static_assert(FunctionPointer<decltype(&makeIntString)>        == true);
	static_assert(FunctionPointer<decltype(&TestClass::makeFloat)> == true);
	static_assert(FunctionPointer<decltype(&TestClass::constFunc)> == true);
	static_assert(FunctionPointer<decltype(&TestClass::opFloats)>  == true);

	static_assert(MemberFunctionPointer<int>                             == false);
	static_assert(MemberFunctionPointer<std::string>                     == false);
	static_assert(MemberFunctionPointer<TestClass>                       == false);
	static_assert(MemberFunctionPointer<decltype(&makeIntString)>        == false);
	static_assert(MemberFunctionPointer<decltype(&TestClass::makeFloat)> == true);
	static_assert(MemberFunctionPointer<decltype(&TestClass::constFunc)> == true);
	static_assert(MemberFunctionPointer<decltype(&TestClass::opFloats)>  == true);

	static_assert(StaticFunctionPointer<int>                             == false);
	static_assert(StaticFunctionPointer<std::string>                     == false);
	static_assert(StaticFunctionPointer<TestClass>                       == false);
	static_assert(StaticFunctionPointer<decltype(&makeIntString)>        == true);
	static_assert(StaticFunctionPointer<decltype(&TestClass::makeFloat)> == false);
	static_assert(StaticFunctionPointer<decltype(&TestClass::constFunc)> == false);
	static_assert(StaticFunctionPointer<decltype(&TestClass::opFloats)>  == false);

	static_assert(CallableTraits<TestClass>::arg_count == 1);
	static_assert(std::same_as<void, CallableTraits<TestClass>::return_t>);
	static_assert(std::same_as<int, CallableTraits<TestClass>::nth_arg_t<0>>);

	constexpr CallableTraits traits(&TestClass::makeFloat);

	{
		using func_t = decltype(&makeIntString);

		static_assert(CallableTraits<func_t>::arg_count == 1);
		static_assert(std::same_as<std::string, CallableTraits<func_t>::return_t>);
		static_assert(std::same_as<int, CallableTraits<func_t>::nth_arg_t<0>>);
		static_assert(std::same_as<int, CallableTraits<func_t>::nth_arg_t<0>>);

		static_assert( !(CallableTraits<func_t>::arg_count == 2));
		static_assert( !std::same_as<Number, CallableTraits<func_t>::return_t>);
		static_assert( !std::same_as<float, CallableTraits<func_t>::nth_arg_t<0>>);
	}

	{
		using func_t = decltype(&TestClass::makeString);
		using std_func_apply = CallableTraits<func_t>::apply_signature<std::function>;

		static_assert(CallableTraits<func_t>::arg_count == 1);
		static_assert(std::same_as<std::string, CallableTraits<func_t>::return_t>);
		static_assert(std::same_as<const char*, CallableTraits<func_t>::nth_arg_t<0>>);
		static_assert(std::same_as<void, CallableTraits<func_t>::target_type>);
		static_assert(std::same_as<std_func_apply, std::function<std::string(const char*)>>);

		static_assert(!(CallableTraits<func_t>::arg_count == 2));
		static_assert(!std::same_as<Number, CallableTraits<func_t>::return_t>);
		static_assert(!std::same_as<float, CallableTraits<func_t>::nth_arg_t<0>>);
	}

	{
		using func_t = decltype(&TestClass::makeFloat);

		static_assert(CallableTraits<func_t>::arg_count == 2);
		static_assert(std::same_as<float, CallableTraits<func_t>::return_t>);
		static_assert(std::same_as<int, CallableTraits<func_t>::nth_arg_t<0>>);
		static_assert(std::same_as<float, CallableTraits<func_t>::nth_arg_t<1>>);

		static_assert(!(CallableTraits<func_t>::arg_count == 1));
		static_assert(!std::same_as<Number, CallableTraits<func_t>::return_t>);
		static_assert(!std::same_as<float, CallableTraits<func_t>::nth_arg_t<0>>);
		static_assert(!std::same_as<std::string, CallableTraits<func_t>::nth_arg_t<1>>);
	}
}