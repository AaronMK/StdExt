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
	constexpr int value() const
	{
		return 1;
	}
};

template<>
class NameClass<"two">
{
public:
	constexpr int value() const
	{
		return 2;
	}
};

void testConst()
{
	static_assert(11 == Const::strLength("Test String"));
	constexpr Const::String strTwo = "two";

	static_assert( std::same_as<NameClass<"two">, NameClass<"two">> );
	static_assert( std::same_as<NameClass<"two">, NameClass<strTwo>> );
	static_assert( !std::same_as<NameClass<"one">, NameClass<"two">> );

	NameClass<"two"> two;
	static_assert(2 == two.value());

	NameClass<"one"> one;
	static_assert(1 == one.value());

	NameClass<"char count"> zero;
	static_assert(10 == zero.value());
}
