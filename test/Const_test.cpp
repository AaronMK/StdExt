#include <StdExt/String.h>

using namespace StdExt;

template<Const::String p_name>
class NameClass
{
public:
	constexpr auto value() const
	{
		return p_name.size();
	}
};

template<>
class NameClass<"one">
{
public:
	int value() const
	{
		return 1;
	}
};

template<>
class NameClass<"two">
{
public:
	int value() const
	{
		return 2;
	}
};

void testConst()
{
	constexpr size_t str_len = Const::strLength("Test String");
	constexpr Const::String strTwo = "two";

	static_assert( std::same_as<NameClass<"two">, NameClass<"two">> );
	static_assert( std::same_as<NameClass<"two">, NameClass<strTwo>> );
	static_assert( !std::same_as<NameClass<"one">, NameClass<"two">> );

	NameClass<"two"> two;
	int int_two_val = two.value();

	NameClass<"one"> one;
	int int_one = one.value();

	NameClass<"zero"> zero;
	size_t int_zero = zero.value();
}