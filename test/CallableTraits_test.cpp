#include <StdExt/FunctionTraits.h>
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


	static_assert(std::same_as<
		Function<&foo>::arg_types, Types<const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_static>::arg_types,
		Types<const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_a>::arg_types,
		Types<const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_b>::arg_types,
		Types<const std::string&, int>
	>);

	static_assert(Function<&foo>::arg_count == 2);
	static_assert(Function<&Bar::func_static>::arg_count == 2);
	static_assert(Function<&Bar::func_a>::arg_count == 2);
	static_assert(Function<&Bar::func_b>::arg_count == 2);

	static_assert(std::same_as< Function<&foo>::nth_arg_t<0>, const std::string&>);
	static_assert(std::same_as< Function<&Bar::func_static>::nth_arg_t<0>, const std::string&>);
	static_assert(std::same_as< Function<&Bar::func_a>::nth_arg_t<0>, const std::string&>);
	static_assert(std::same_as< Function<&Bar::func_b>::nth_arg_t<0>, const std::string&>);

	static_assert(std::same_as< Function<&foo>::nth_arg_t<1>, int>);
	static_assert(std::same_as< Function<&Bar::func_static>::nth_arg_t<1>, int>);
	static_assert(std::same_as< Function<&Bar::func_a>::nth_arg_t<1>, int>);
	static_assert(std::same_as< Function<&Bar::func_b>::nth_arg_t<1>, int>);

	static_assert(std::same_as<
		Function<&foo>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_static>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_a>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_b>::forward<std::tuple>,
		std::tuple<int, const std::string&, int>
	>);

	static_assert(std::same_as<
		Function<&foo>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_static>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_a>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_b>::forward<std::tuple, char>,
		std::tuple<char, int, const std::string&, int>
	>);

	static_assert(std::same_as<
		Function<&foo>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_static>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_a>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);
	static_assert(std::same_as<
		Function<&Bar::func_b>::apply_signature<std::function>,
		std::function<int(const std::string&, int)>
	>);

	static_assert(std::same_as< Function<&foo>::class_type, void>);
	static_assert(std::same_as< Function<&Bar::func_static>::class_type, void>);
	static_assert(std::same_as< Function<&Bar::func_a>::class_type, Bar>);
	static_assert(std::same_as< Function<&Bar::func_b>::class_type, Bar>);

	static_assert(std::same_as< Function<&foo>::target_type, void>);
	static_assert(std::same_as< Function<&Bar::func_static>::target_type, void>);
	static_assert(std::same_as< Function<&Bar::func_a>::target_type, Bar*>);
	static_assert(std::same_as< Function<&Bar::func_b>::target_type, const Bar*>);

	static_assert(Callable<int>         == false);
	static_assert(Callable<std::string> == false);
	static_assert(Callable<TestClass>   == true);

	static_assert(Function<TestClass>::arg_count == 1);
	static_assert(std::same_as<void, Function<TestClass>::return_type>);
	static_assert(std::same_as<int, Function<TestClass>::nth_arg_t<0>>);

	constexpr CallableTraits traits(&TestClass::makeFloat);

	{
		using func_t = decltype(&makeIntString);

		static_assert(Function<func_t>::arg_count == 1);
		static_assert(std::same_as<std::string, Function<func_t>::return_type>);
		static_assert(std::same_as<int, Function<func_t>::nth_arg_t<0>>);
		static_assert(std::same_as<int, Function<func_t>::nth_arg_t<0>>);

		static_assert( !(Function<func_t>::arg_count == 2));
		static_assert( !std::same_as<Number, Function<func_t>::return_type>);
		static_assert( !std::same_as<float, Function<func_t>::nth_arg_t<0>>);
	}

	{
		using func_t = decltype(&TestClass::makeString);
		using std_func_apply = Function<func_t>::apply_signature<std::function>;

		static_assert(Function<func_t>::arg_count == 1);
		static_assert(std::same_as<std::string, Function<func_t>::return_type>);
		static_assert(std::same_as<const char*, Function<func_t>::nth_arg_t<0>>);
		static_assert(std::same_as<void, Function<func_t>::target_type>);
		static_assert(std::same_as<std_func_apply, std::function<std::string(const char*)>>);

		static_assert(!(Function<func_t>::arg_count == 2));
		static_assert(!std::same_as<Number, Function<func_t>::return_type>);
		static_assert(!std::same_as<float, Function<func_t>::nth_arg_t<0>>);
	}

	{
		using func_t = decltype(&TestClass::makeFloat);

		static_assert(Function<func_t>::arg_count == 2);
		static_assert(std::same_as<float, Function<func_t>::return_type>);
		static_assert(std::same_as<int, Function<func_t>::nth_arg_t<0>>);
		static_assert(std::same_as<float, Function<func_t>::nth_arg_t<1>>);

		static_assert(!(Function<func_t>::arg_count == 1));
		static_assert(!std::same_as<Number, Function<func_t>::return_type>);
		static_assert(!std::same_as<float, Function<func_t>::nth_arg_t<0>>);
		static_assert(!std::same_as<std::string, Function<func_t>::nth_arg_t<1>>);
	}
}