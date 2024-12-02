#include <StdExt/CallableTraits.h>
#include <StdExt/Number.h>

#include <string>
#include <functional>

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

class TestDrived : public TestClass
{
public:
	int opFloats(float left, float right) override
	{
		return left * right;
	}
};

static std::string makeIntString(int i)
{
	return std::to_string(i);
}

void testCallableTraits()
{

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
	auto triats = CallableTraits(&TestClass::opFloats);

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
		using std_func_apply = CallableTraits<func_t>::apply_signiture<std::function>;

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