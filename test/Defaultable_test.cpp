#include <StdExt/Defaultable.h>

#include <concepts>

using namespace StdExt;

template<int32_t default_val = 0>
using Int = DefaultableMember<int32_t, default_val>;

template<uint16_t default_val = 0>
using Uint = DefaultableMember<uint16_t, default_val>;

struct DefaultableClass
{
	Int<1>    field_one;
	Int<2>    field_two;
	Uint<33>  field_33;
};

template<auto left_default_val, decltype(left_default_val) left_alt_val, auto right_default_val>
void testOperators()
{
	using left_t  = decltype(left_default_val);
	using right_t = decltype(right_default_val);

	using LeftDef = DefaultableMember<left_t, left_default_val>;
	using RightDef = DefaultableMember<right_t, right_default_val>;

	// Type equivalency testing of operators for mixed template parameters.
	static_assert( std::same_as<decltype(LeftDef() +   RightDef()), decltype(left_default_val +   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() -   RightDef()), decltype(left_default_val -   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() *   RightDef()), decltype(left_default_val *   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() /   RightDef()), decltype(left_default_val /   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() <   RightDef()), decltype(left_default_val <   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() <=  RightDef()), decltype(left_default_val <=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() ==  RightDef()), decltype(left_default_val ==  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() !=  RightDef()), decltype(left_default_val !=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() >=  RightDef()), decltype(left_default_val >=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() >   RightDef()), decltype(left_default_val >   right_default_val)> );

	// Type equivalency testing of operators for left fundamental parameters.
	static_assert( std::same_as<decltype(left_default_val +   RightDef()), decltype(left_default_val +   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val -   RightDef()), decltype(left_default_val -   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val *   RightDef()), decltype(left_default_val *   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val /   RightDef()), decltype(left_default_val /   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val <   RightDef()), decltype(left_default_val <   right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val <=  RightDef()), decltype(left_default_val <=  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val ==  RightDef()), decltype(left_default_val ==  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val !=  RightDef()), decltype(left_default_val !=  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val >=  RightDef()), decltype(left_default_val >=  right_default_val)> );
	static_assert( std::same_as<decltype(left_default_val >   RightDef()), decltype(left_default_val >   right_default_val)> );
	
	// Type equivalency testing of operators for right fundamental parameters.
	static_assert( std::same_as<decltype(LeftDef() +   right_default_val), decltype(left_default_val +   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() -   right_default_val), decltype(left_default_val -   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() *   right_default_val), decltype(left_default_val *   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() /   right_default_val), decltype(left_default_val /   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() <   right_default_val), decltype(left_default_val <   right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() <=  right_default_val), decltype(left_default_val <=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() ==  right_default_val), decltype(left_default_val ==  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() !=  right_default_val), decltype(left_default_val !=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() >=  right_default_val), decltype(left_default_val >=  right_default_val)> );
	static_assert( std::same_as<decltype(LeftDef() >   right_default_val), decltype(left_default_val >   right_default_val)> );

	// Result equivalency testing of operators for mixed template parameters.
	static_assert( (LeftDef() +   RightDef()) == (left_default_val +   right_default_val) );
	static_assert( (LeftDef() -   RightDef()) == (left_default_val -   right_default_val) );
	static_assert( (LeftDef() *   RightDef()) == (left_default_val *   right_default_val) );
	static_assert( (LeftDef() /   RightDef()) == (left_default_val /   right_default_val) );
	static_assert( (LeftDef() <   RightDef()) == (left_default_val <   right_default_val) );
	static_assert( (LeftDef() <=  RightDef()) == (left_default_val <=  right_default_val) );
	static_assert( (LeftDef() ==  RightDef()) == (left_default_val ==  right_default_val) );
	static_assert( (LeftDef() !=  RightDef()) == (left_default_val !=  right_default_val) );
	static_assert( (LeftDef() >=  RightDef()) == (left_default_val >=  right_default_val) );
	static_assert( (LeftDef() >   RightDef()) == (left_default_val >   right_default_val) );

	// Result equivalency testing of operators for left fundamental parameters.
	static_assert( (left_default_val +   RightDef()) == (left_default_val +   right_default_val) );
	static_assert( (left_default_val -   RightDef()) == (left_default_val -   right_default_val) );
	static_assert( (left_default_val *   RightDef()) == (left_default_val *   right_default_val) );
	static_assert( (left_default_val /   RightDef()) == (left_default_val /   right_default_val) );
	static_assert( (left_default_val <   RightDef()) == (left_default_val <   right_default_val) );
	static_assert( (left_default_val <=  RightDef()) == (left_default_val <=  right_default_val) );
	static_assert( (left_default_val ==  RightDef()) == (left_default_val ==  right_default_val) );
	static_assert( (left_default_val !=  RightDef()) == (left_default_val !=  right_default_val) );
	static_assert( (left_default_val >=  RightDef()) == (left_default_val >=  right_default_val) );
	static_assert( (left_default_val >   RightDef()) == (left_default_val >   right_default_val) );
	
	// Result equivalency testing of operators for right fundamental parameters.
	static_assert( (LeftDef() +   right_default_val) == (left_default_val +   right_default_val) );
	static_assert( (LeftDef() -   right_default_val) == (left_default_val -   right_default_val) );
	static_assert( (LeftDef() *   right_default_val) == (left_default_val *   right_default_val) );
	static_assert( (LeftDef() /   right_default_val) == (left_default_val /   right_default_val) );
	static_assert( (LeftDef() <   right_default_val) == (left_default_val <   right_default_val) );
	static_assert( (LeftDef() <=  right_default_val) == (left_default_val <=  right_default_val) );
	static_assert( (LeftDef() ==  right_default_val) == (left_default_val ==  right_default_val) );
	static_assert( (LeftDef() !=  right_default_val) == (left_default_val !=  right_default_val) );
	static_assert( (LeftDef() >=  right_default_val) == (left_default_val >=  right_default_val) );
	static_assert( (LeftDef() >   right_default_val) == (left_default_val >   right_default_val) );

	if constexpr ( ThreeWayComperableWith<left_t, right_t> )
	{
		static_assert( std::same_as<decltype(LeftDef() <=> RightDef()), decltype(left_default_val <=> right_default_val)> );
		static_assert( std::same_as<decltype(left_default_val <=> RightDef()), decltype(left_default_val <=> right_default_val)> );
		static_assert( std::same_as<decltype(LeftDef() <=> right_default_val), decltype(left_default_val <=> right_default_val)> );
		static_assert( (LeftDef() <=> right_default_val) == (left_default_val <=> right_default_val) );
		static_assert( (left_default_val <=> RightDef()) == (left_default_val <=> right_default_val) );
		static_assert( (LeftDef() <=> RightDef()) == (left_default_val <=> right_default_val) );
	}
}

void testDefaultable()
{
	constexpr uint32_t left_default  = 1;
	constexpr int64_t  right_default = 50;
	testOperators<left_default, 75, right_default>();

	DefaultableMember<int64_t, 50> DM50;
	DefaultableMember<uint32_t, 1> DM1;
	auto test_compare = DM50 <=> DM1;

	DefaultableClass dc;
	dc.field_one = 5;
	dc.field_two = 7;

	auto add_test = dc.field_one + dc.field_33;

	DefaultableClass dc_next(std::move(dc));
	dc.field_one = dc.field_two;
	dc.field_33 = 1.4;

	dc_next.field_one = std::move(dc.field_33);

	auto inc_test = ++dc.field_one;
}