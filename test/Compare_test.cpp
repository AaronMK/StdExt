#include <StdExt/Compare.h>

#include <string>

using namespace StdExt;

class TestNoComp
{
private:
	int A;
	int B;

public:
	TestNoComp()
		: A(0), B(0)
	{
	}
};

class TestDefaultStrongComp
{
private:
	int A;
	int B;

public:
	TestDefaultStrongComp()
		: A(0), B(0)
	{
	}

	constexpr auto operator<=>(const TestDefaultStrongComp&) const = default;
};

class TestWeakCompWithEquality
{
private:
	int A;
	int B;

public:
	consteval TestWeakCompWithEquality()
		: A(0), B(0)
	{
	}

	constexpr TestWeakCompWithEquality(int _A, int _B)
		: A(_A), B(_B)
	{
	}

	constexpr std::weak_ordering operator<=>(const TestWeakCompWithEquality& other) const
	{
		return std::weak_ordering(A <=> other.A);
	}

	constexpr bool operator==(const TestWeakCompWithEquality& other) const
	{
		return A == other.A;
	}
};

class TestWeakCompNoEquality
{
private:
	int A;
	int B;

public:
	consteval TestWeakCompNoEquality()
		: A(0), B(0)
	{
	}

	constexpr TestWeakCompNoEquality(int _A, int _B)
		: A(_A), B(_B)
	{
	}

	constexpr std::weak_ordering operator<=>(const TestWeakCompNoEquality& other) const
	{
		return std::weak_ordering(A <=> other.A);
	}
};

class TestPartialCompDefault
{
private:
	int         A;
	float       B;
	std::string C;

public:
	consteval TestPartialCompDefault()
		: A(0), B(0.0f)
	{
	}

	constexpr TestPartialCompDefault(int _A, float _B)
		: A(_A), B(_B)
	{
	}

	constexpr auto operator<=>(const TestPartialCompDefault& other) const = default;
};

void testCompare()
{
	static_assert(  OrderingClass<std::weak_ordering> );
	static_assert(  OrderingClass<std::partial_ordering> );
	static_assert(  OrderingClass<std::strong_ordering> );
	static_assert( !OrderingClass<int> );
	static_assert( !OrderingClass<std::string> );

	static_assert(
		std::same_as<
			std::compare_three_way_result_t<TestPartialCompDefault>,
			UniformOrderingResult<int, float, std::string>
		>
	);
}