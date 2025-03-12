#include <StdExt/Compare.h>

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
	int A;
	float B;

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
	static_assert( !HasLessThan<TestNoComp> );
	static_assert( !HasLessThanEqual<TestNoComp> );
	static_assert( !HasEquals<TestNoComp> );
	static_assert( !HasNotEqual<TestNoComp> );
	static_assert( !HasGreaterThanEqual<TestNoComp> );
	static_assert( !HasGreaterThan<TestNoComp> );

	static_assert( HasLessThan<TestDefaultStrongComp> );
	static_assert( HasLessThanEqual<TestDefaultStrongComp> );
	static_assert( HasEquals<TestDefaultStrongComp> );
	static_assert( HasNotEqual<TestDefaultStrongComp> );
	static_assert( HasGreaterThanEqual<TestDefaultStrongComp> );
	static_assert( HasGreaterThan<TestDefaultStrongComp> );

	static_assert(  HasLessThan<TestWeakCompWithEquality> );
	static_assert(  HasLessThanEqual<TestWeakCompWithEquality> );
	static_assert(  HasEquals<TestWeakCompWithEquality> );
	static_assert(  HasNotEqual<TestWeakCompWithEquality> );
	static_assert(  HasGreaterThanEqual<TestWeakCompWithEquality> );
	static_assert(  HasGreaterThan<TestWeakCompWithEquality> );

	static_assert(  HasLessThan<TestWeakCompNoEquality> );
	static_assert(  HasLessThanEqual<TestWeakCompNoEquality> );
	static_assert( !HasEquals<TestWeakCompNoEquality> );
	static_assert( !HasNotEqual<TestWeakCompNoEquality> );
	static_assert(  HasGreaterThanEqual<TestWeakCompNoEquality> );
	static_assert(  HasGreaterThan<TestWeakCompNoEquality> );

	static_assert(  HasLessThan<TestPartialCompDefault> );
	static_assert(  HasLessThanEqual<TestPartialCompDefault> );
	static_assert(  HasEquals<TestPartialCompDefault> );
	static_assert(  HasNotEqual<TestPartialCompDefault> );
	static_assert(  HasGreaterThanEqual<TestPartialCompDefault> );
	static_assert(  HasGreaterThan<TestPartialCompDefault> );

	static_assert(  ThreeWayComperableWith<int16_t, int32_t> );
	static_assert( !ThreeWayComperableWith<uint64_t, int32_t> );

	using comp_t = decltype(std::declval<TestPartialCompDefault>() <=> std::declval<TestPartialCompDefault>());
}