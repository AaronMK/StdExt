#include <StdExt/CallableTraits.h>
#include <StdExt/Number.h>

#include <string>

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
};

static std::string makeIntString(int i)
{
	return std::to_string(i);
}

void testFunctionTraits()
{
	static_assert(CallableTraits<TestClass>::arg_count == 1);
	static_assert(std::same_as<void, CallableTraits<TestClass>::return_t>);
	static_assert(std::same_as<int, CallableTraits<TestClass>::nth_arg_t<0>>);

	{
		using func_t = decltype(&makeIntString);

		static_assert(CallableTraits<func_t>::arg_count == 1);
		static_assert(std::same_as<std::string, CallableTraits<func_t>::return_t>);
		static_assert(std::same_as<int, CallableTraits<func_t>::nth_arg_t<0>>);

		static_assert( !(CallableTraits<func_t>::arg_count == 2));
		static_assert( !std::same_as<Number, CallableTraits<func_t>::return_t>);
		static_assert( !std::same_as<float, CallableTraits<func_t>::nth_arg_t<0>>);
	}

	{
		using func_t = decltype(&TestClass::makeString);

		static_assert(CallableTraits<func_t>::arg_count == 1);
		static_assert(std::same_as<std::string, CallableTraits<func_t>::return_t>);
		static_assert(std::same_as<const char*, CallableTraits<func_t>::nth_arg_t<0>>);

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