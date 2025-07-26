#include <StdExt/Defaultable.h>
#include <StdExt/Test/Test.h>

#include <concepts>

using namespace StdExt;

template<int32_t default_val = 0>
using Int = DefaultableMember<int32_t, default_val>;

template<uint16_t default_val = 0>
using Uint = DefaultableMember<uint16_t, default_val>;

template<float default_val = 0.0f>
using Float = DefaultableMember<float, default_val>;

struct DefaultableClass
{
	Int<1>      field_one;
	Int<2>      field_two;
	Uint<33>    field_33;
	Float<5.5f> field_55;
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
	using namespace StdExt::Test;

	constexpr uint32_t left_default_1  = 1;
	constexpr int64_t  right_default_1 = 50;
	testOperators<left_default_1, 75, right_default_1>();

	constexpr float left_default_2  = 1.0f;
	constexpr int64_t  right_default_2 = 50;
	testOperators<left_default_2, 75.0f, right_default_2>();
	
	constexpr float left_default_3  = 1.0f;
	constexpr uint64_t  right_default_3 = 50;
	testOperators<left_default_3, 75.0f, right_default_3>();

	Float<3.0f> float_3_0;

	testForResult<float>("Defaultable value has expected initial value on default construction.", float_3_0, 3.0f);
	testForResult<float>("Defaultable value has expected default_value.", float_3_0.default_value, 3.0f);
	testForResult<float>("Defaultable value has takes the value of its constructor parameter.", Float<3.0f>(4.0f), 4.0f);

	float_3_0 = 4.0f;
	testForResult<float>("Defaultable accepts a new value.", float_3_0, 4.0f);

	Float<3.0f> copy_target(float_3_0);
	testForResult<float>("Defaultable retains its value after being the source of a copy construction.", float_3_0, 4.0f);
	testForResult<float>("Defaultable takes the value of a copied Defaultable when the target of a copy construction.", copy_target, 4.0f);

	copy_target = copy_target.default_value;
	copy_target = float_3_0;

	testForResult<float>("Defaultable retains its value after being the source of a copy assignment.", float_3_0, 4.0f);
	testForResult<float>("Defaultable takes the value of a copied Defaultable when the target of a copy assignment.", copy_target, 4.0f);

	Float<3.0f> move_target(std::move(float_3_0));
	testForResult<float>("Defaultable returns to default value after being the source of a move constructor.", float_3_0, 3.0f);
	testForResult<float>("Defaultable takes the value of a moved Defaultable when the target of a move constructor.", move_target, 4.0f);

	float_3_0 = 4.0f;
	move_target = move_target.default_value;

	move_target = std::move(float_3_0);
	testForResult<float>("Defaultable returns to default value after being the source of a move assignment.", float_3_0, 3.0f);
	testForResult<float>("Defaultable takes the value of a moved Defaultable when the target of a move assignment.", move_target, 4.0f);

	DefaultableClass DC_orig;
	DC_orig.field_33 = 22;
	DC_orig.field_55 = 1.1f;
	DC_orig.field_one = 20;
	DC_orig.field_two = 30;

	DefaultableClass DC_copy(DC_orig);

	testForResult<bool>(
		"Defaultable members of class retain values when parent object is the source of a default copy constructor.", true,
		22 == DC_orig.field_33 && 1.1f == DC_orig.field_55 && 20 == DC_orig.field_one && 30 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default copy constructor.", true,
		22 == DC_copy.field_33 && 1.1f == DC_copy.field_55 && 20 == DC_copy.field_one && 30 == DC_copy.field_two
	);

	DefaultableClass DC_move(std::move(DC_orig));

	testForResult<bool>(
		"Defaultable members of class return to default values when parent object is the source of a default move constructor.", true,
		33 == DC_orig.field_33 && 5.5f == DC_orig.field_55 && 1 == DC_orig.field_one && 2 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default move constructor.", true,
		22 == DC_move.field_33 && 1.1f == DC_move.field_55 && 20 == DC_move.field_one && 30 == DC_move.field_two
	);

	DC_orig.field_33 = 22;
	DC_orig.field_55 = 1.1f;
	DC_orig.field_one = 20;
	DC_orig.field_two = 30;
	DC_copy = DC_orig;

	testForResult<bool>(
		"Defaultable members of class retain values when parent object is the source of a default copy assignment.", true,
		22 == DC_orig.field_33 && 1.1f == DC_orig.field_55 && 20 == DC_orig.field_one && 30 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default copy assignment.", true,
		22 == DC_copy.field_33 && 1.1f == DC_copy.field_55 && 20 == DC_copy.field_one && 30 == DC_copy.field_two
	);

	DC_move = std::move(DC_orig);

	testForResult<bool>(
		"Defaultable members of class return to default values when parent object is the source of a default move operation.", true,
		33 == DC_orig.field_33 && 5.5f == DC_orig.field_55 && 1 == DC_orig.field_one && 2 == DC_orig.field_two
	);

	testForResult<bool>(
		"Defaultable members of class get source values when parent object is the destination of a default move assignment.", true,
		22 == DC_move.field_33 && 1.1f == DC_move.field_55 && 20 == DC_move.field_one && 30 == DC_move.field_two
	);
}