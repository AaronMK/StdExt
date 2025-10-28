#include <StdExt/Operators.h>

#include <StdExt/Test/Test.h>

#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

using namespace StdExt;

void testOperators()
{
	#pragma region Validity

	static_assert(  Plus<const uint64_t&, uint32_t&>::is_valid );
	static_assert(  Minus<const uint64_t&, uint32_t&>::is_valid );
	static_assert(  Multiply<const uint64_t&, uint32_t&>::is_valid );
	static_assert(  Divide<const uint64_t&, uint32_t&>::is_valid );

	static_assert( !AssignPlus<const uint64_t&, uint32_t&>::is_valid );
	static_assert( !AssignMinus<const uint64_t&, uint32_t&>::is_valid );
	static_assert( !AssignMultiply<const uint64_t&, uint32_t&>::is_valid );
	static_assert( !AssignDivide<const uint64_t&, uint32_t&>::is_valid );

	static_assert( Plus<const int*, int>::is_valid );
	static_assert( Minus<const int*, int>::is_valid );
	static_assert( And<int*, int>::is_valid );

	static_assert( !Modulus<float, float>::is_valid );
	static_assert(  Modulus<int, int>::is_valid );

	static_assert(  BitwiseAnd<uint32_t, uint64_t>::is_valid );
	static_assert( !BitwiseAnd<std::string, uint64_t>::is_valid );

	static_assert(  BitwiseOr<uint32_t, uint64_t>::is_valid );
	static_assert( !BitwiseOr<std::string, uint64_t>::is_valid );

	static_assert(  BitwiseXor<uint32_t, uint64_t>::is_valid );
	static_assert( !BitwiseXor<std::string, uint64_t>::is_valid );

	static_assert(  ShiftLeft<uint32_t, uint64_t>::is_valid );
	static_assert( !ShiftLeft<std::string, uint64_t>::is_valid );

	static_assert(  ShiftRight<uint32_t, uint64_t>::is_valid );
	static_assert( !ShiftRight<std::string, uint64_t>::is_valid );

	static_assert(  Assign<float, int>::is_valid );
	static_assert( !Assign<const uint64_t&, uint32_t>::is_valid );
	static_assert( !Assign<std::vector<int>, std::string>::is_valid );
	static_assert( !Assign<const std::string, std::string>::is_valid );

	static_assert(  AssignPlus<float, int>::is_valid );
	static_assert( !AssignPlus<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignPlus<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignPlus<const std::string, std::string>::is_valid );

	static_assert(  AssignMinus<float, int>::is_valid );
	static_assert( !AssignMinus<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignMinus<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignMinus<const std::string, std::string>::is_valid );

	static_assert(  AssignMultiply<float, int>::is_valid );
	static_assert( !AssignMultiply<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignMultiply<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignMultiply<const std::string, std::string>::is_valid );

	static_assert(  AssignDivide<float, int>::is_valid );
	static_assert( !AssignDivide<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignDivide<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignDivide<const std::string, std::string>::is_valid );

	static_assert(  AssignModulus<uint64_t, uint32_t>::is_valid );
	static_assert( !AssignModulus<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignModulus<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignModulus<const std::string, std::string>::is_valid );

	static_assert(  AssignBitwiseAnd<uint64_t, uint32_t>::is_valid );
	static_assert( !AssignBitwiseAnd<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignBitwiseAnd<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignBitwiseAnd<const std::string, std::string>::is_valid );

	static_assert(  AssignBitwiseOr<uint64_t, uint32_t>::is_valid );
	static_assert( !AssignBitwiseOr<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignBitwiseOr<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignBitwiseOr<const std::string, std::string>::is_valid );

	static_assert(  AssignBitwiseXor<uint64_t, uint32_t>::is_valid );
	static_assert( !AssignBitwiseXor<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignBitwiseXor<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignBitwiseXor<const std::string, std::string>::is_valid );

	static_assert(  AssignShiftLeft<uint64_t, uint32_t>::is_valid );
	static_assert( !AssignShiftLeft<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignShiftLeft<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignShiftLeft<const std::string, std::string>::is_valid );

	static_assert(  AssignShiftRight<uint64_t, uint32_t>::is_valid );
	static_assert( !AssignShiftRight<const uint64_t&, uint32_t>::is_valid );
	static_assert( !AssignShiftRight<std::vector<int>, std::string>::is_valid );
	static_assert( !AssignShiftRight<const std::string, std::string>::is_valid );

	static_assert(  PrefixIncrement<uint32_t>::is_valid );
	static_assert( !PrefixIncrement<const uint32_t&>::is_valid );
	static_assert( !PrefixIncrement<std::string>::is_valid );

	static_assert(  PostfixIncrement<uint32_t>::is_valid );
	static_assert( !PostfixIncrement<const uint32_t&>::is_valid );
	static_assert( !PostfixIncrement<std::string>::is_valid );

	static_assert(  PrefixDecrement<uint32_t>::is_valid );
	static_assert( !PostfixIncrement<const uint32_t&>::is_valid );
	static_assert( !PrefixDecrement<std::string>::is_valid );

	static_assert(  PostfixDecrement<uint32_t>::is_valid );
	static_assert( !PostfixIncrement<const uint32_t&>::is_valid );
	static_assert( !PostfixDecrement<std::string>::is_valid );

	static_assert(  ThreeWayCompare<std::string>::is_valid );
	static_assert(  LessThan<std::string>::is_valid );
	static_assert(  LessThanEqual<std::string>::is_valid );
	static_assert(  Equal<std::string>::is_valid );
	static_assert(  NotEqual<std::string>::is_valid );
	static_assert(  GreaterThanEqual<std::string>::is_valid );
	static_assert(  GreaterThan<std::string>::is_valid );

	static_assert( !ThreeWayCompare<std::string, float>::is_valid );
	static_assert( !LessThan<std::string, float>::is_valid );
	static_assert( !LessThanEqual<std::string, float>::is_valid );
	static_assert( !Equal<std::string, float>::is_valid );
	static_assert( !NotEqual<std::string, float>::is_valid );
	static_assert( !GreaterThanEqual<std::string, float>::is_valid );
	static_assert( !GreaterThan<std::string, float>::is_valid );

	#pragma endregion

	#pragma region Result Types

	static_assert ( std::same_as<Plus<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<Minus<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<Multiply<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<Divide<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<Modulus<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<BitwiseAnd<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<BitwiseOr<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<BitwiseXor<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<ShiftLeft<const uint64_t&, uint32_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<ShiftRight<const uint64_t&, uint32_t&>::result_type, uint64_t> );

	static_assert ( std::same_as<AssignPlus<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignMinus<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignMultiply<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignDivide<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignModulus<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignBitwiseAnd<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignBitwiseOr<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignBitwiseXor<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignShiftLeft<uint64_t&, const uint32_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<AssignShiftRight<uint64_t&, const uint32_t&>::result_type, uint64_t&> );

	static_assert ( std::same_as<PrefixIncrement<uint64_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<PrefixDecrement<uint64_t&>::result_type, uint64_t&> );
	static_assert ( std::same_as<PostfixIncrement<uint64_t&>::result_type, uint64_t> );
	static_assert ( std::same_as<PostfixDecrement<uint64_t&>::result_type, uint64_t> );

	static_assert( std::same_as<ThreeWayCompare<std::string>::result_type, std::strong_ordering> );
	static_assert( std::same_as<LessThan<std::string>::result_type, bool> );
	static_assert( std::same_as<LessThanEqual<std::string>::result_type, bool> );
	static_assert( std::same_as<Equal<std::string>::result_type, bool> );
	static_assert( std::same_as<NotEqual<std::string>::result_type, bool> );
	static_assert( std::same_as<GreaterThanEqual<std::string>::result_type, bool> );
	static_assert( std::same_as<GreaterThan<std::string>::result_type, bool> );

	#pragma endregion

	#pragma region Result Checks

	constexpr auto FloatPlus        = Plus<float>();
	constexpr auto FloatMinus       = Minus<float>();
	constexpr auto FloatMultiply    = Multiply<float>();
	constexpr auto FloatDivide      = Divide<float>();
	constexpr auto IntModulus       = Modulus<int>();
	constexpr auto IntBitAnd        = BitwiseAnd<int>();
	constexpr auto IntBitOr         = BitwiseOr<int>();
	constexpr auto IntBitXor        = BitwiseXor<int>();
	constexpr auto IntBitShiftLeft  = ShiftLeft<int>();
	constexpr auto IntBitShiftRight = ShiftRight<int>();
	constexpr auto BoolAnd          = And<bool>();
	constexpr auto BoolOr           = Or<bool>();

	constexpr auto FloatIntPlus     = Plus<float, int>();
	constexpr auto FloatIntMinus    = Minus<float, int>();
	constexpr auto FloatIntMultiply = Multiply<float, int>();
	constexpr auto FloatIntDivide   = Divide<float, int>();
	constexpr auto MixModulus       = Modulus<int32_t, int64_t>();
	constexpr auto MixBitAnd        = BitwiseAnd<int32_t, int64_t>();
	constexpr auto MixBitOr         = BitwiseOr<int32_t, int64_t>();
	constexpr auto MixBitXor        = BitwiseXor<int32_t, int64_t>();
	constexpr auto MixBitShiftLeft  = ShiftLeft<int32_t, int64_t>();
	constexpr auto MixBitShiftRight = ShiftRight<int32_t, int64_t>();
	constexpr auto MixAnd           = And<bool, int>();
	constexpr auto MixOr            = Or<bool, int>();

	static_assert( FloatPlus(2.0f, 1.0f) == 3.0f);
	static_assert( FloatMinus(3.0f, 2.0f) == 1.0f);
	static_assert( FloatMultiply(2.0f, 3.0f) == 6.0f);
	static_assert( FloatDivide(5.0f, 2.0f) == 2.5f);
	static_assert( IntModulus(11, 3) == 2);
	static_assert( IntBitAnd(7, 6) == 6);
	static_assert( IntBitOr(9, 7) == 15);
	static_assert( IntBitXor(10, 6) == 12);
	static_assert( IntBitShiftLeft(8, 2) == 32);
	static_assert( IntBitShiftRight(8, 2) == 2);
	static_assert( BoolAnd(true, false) == false);
	static_assert( BoolOr(false, true) == true);

	static_assert( FloatIntPlus(2.0f, 1) == 3.0f);
	static_assert( FloatIntMinus(3.0f, 2) == 1.0f);
	static_assert( FloatIntMultiply(2.0f, 3) == 6.0f);
	static_assert( FloatIntDivide(5.0f, 2) == 2.5f);
	static_assert( MixModulus(11, 3) == 2);
	static_assert( MixBitAnd(7, 6) == 6);
	static_assert( MixBitOr(9, 7) == 15);
	static_assert( MixBitXor(10, 6) == 12);
	static_assert( MixBitShiftLeft(8, 2) == 32);
	static_assert( MixBitShiftRight(8, 2) == 2);
	static_assert( MixAnd(true, 0) == false);
	static_assert( MixOr(false, 1) == true);

	constexpr auto PreInc              = PrefixIncrement<int64_t&>();
	constexpr auto PreDec              = PrefixDecrement<int64_t&>();
	constexpr auto PostInc             = PostfixIncrement<int64_t&>();
	constexpr auto PostDec             = PostfixDecrement<int64_t&>();

	constexpr auto AssignPlusInt       = AssignPlus<int64_t&, int32_t>();
	constexpr auto AssignMinusInt      = AssignMinus<int64_t&, int32_t>();
	constexpr auto AssignMultiplyInt   = AssignMultiply<int64_t&, int32_t>();
	constexpr auto AssignDivideInt     = AssignDivide<int64_t&, int32_t>();
	constexpr auto AssignShiftLeftInt  = AssignShiftLeft<int64_t&, int32_t>();
	constexpr auto AssignShiftRightInt = AssignShiftRight<int64_t&, int32_t>();
	constexpr auto AssignBitAndInt     = AssignBitwiseAnd<int64_t&, int32_t>();
	constexpr auto AssignBitOrInt      = AssignBitwiseOr<int64_t&, int32_t>();
	constexpr auto AssignBitXorInt     = AssignBitwiseXor<int64_t&, int32_t>();

	int64_t IntDecSrc = 5;

	Test::testForResult<int64_t>("PrefixIncrement: Returns post increment value.", PreInc(IntDecSrc), 6);
	Test::testForResult<int64_t>("PrefixIncrement: Actual value of variable has changed as expected.", IntDecSrc, 6);

	Test::testForResult<int64_t>("PrefixDecrement: Returns post decrement value.", PreDec(IntDecSrc), 5);
	Test::testForResult<int64_t>("PrefixDecrement: Actual value of variable has changed as expected.", IntDecSrc, 5);

	Test::testForResult<int64_t>("PostfixIncrement: Returns pre increment value.", PostInc(IntDecSrc), 5);
	Test::testForResult<int64_t>("PostfixIncrement: Actual value of variable has changed as expected.", IntDecSrc, 6);

	Test::testForResult<int64_t>("PostfixDecrement: Returns pre decrement value.", PostDec(IntDecSrc), 6);
	Test::testForResult<int64_t>("PostfixDecrement: Actual value of variable has changed as expected.", IntDecSrc, 5);

	Test::testForResult<int64_t>("AssignPlus: Returns post addition value.", AssignPlusInt(IntDecSrc, 2), 7);
	Test::testForResult<int64_t>("AssignPlus: Actual value of variable has changed as expected.", IntDecSrc, 7);

	Test::testForResult<int64_t>("AssignMinus: Returns post subtraction value.", AssignMinusInt(IntDecSrc, 2), 5);
	Test::testForResult<int64_t>("AssignMinus: Actual value of variable has changed as expected.", IntDecSrc, 5);

	Test::testForResult<int64_t>("AssignMultiply: Returns post multiplication value.", AssignMultiplyInt(IntDecSrc, 2), 10);
	Test::testForResult<int64_t>("AssignMultiply: Actual value of variable has changed as expected.", IntDecSrc, 10);

	Test::testForResult<int64_t>("AssignDivide: Returns post division value.", AssignDivideInt(IntDecSrc, 2), 5);
	Test::testForResult<int64_t>("AssignDivide: Actual value of variable has changed as expected.", IntDecSrc, 5);

	Test::testForResult<int64_t>("AssignShiftLeft: Returns post shift left value.", AssignShiftLeftInt(IntDecSrc, 2), 20);
	Test::testForResult<int64_t>("AssignShiftLeft: Actual value of variable has changed as expected.", IntDecSrc, 20);

	Test::testForResult<int64_t>("AssignShiftRight: Returns post shift right value.", AssignShiftRightInt(IntDecSrc, 2), 5);
	Test::testForResult<int64_t>("AssignShiftRight: Actual value of variable has changed as expected.", IntDecSrc, 5);

	IntDecSrc = 7;
	Test::testForResult<int64_t>("AssignBitwiseAnd: Returns post bitwise and value.", AssignBitAndInt(IntDecSrc, 6), 6);
	Test::testForResult<int64_t>("AssignBitwiseAnd: Actual value of variable has changed as expected.", IntDecSrc, 6);

	IntDecSrc = 9;
	Test::testForResult<int64_t>("AssignBitwiseOr: Returns post bitwise or value.", AssignBitOrInt(IntDecSrc, 7), 15);
	Test::testForResult<int64_t>("AssignBitwiseOr: Actual value of variable has changed as expected.", IntDecSrc, 15);

	IntDecSrc = 10;
	Test::testForResult<int64_t>("AssignBitwiseXor: Returns post bitwise xor value.", AssignBitXorInt(IntDecSrc, 6), 12);
	Test::testForResult<int64_t>("AssignBitwiseXor: Actual value of variable has changed as expected.", IntDecSrc, 12);

	#pragma endregion

	#pragma region CallablePointer Conversion;

	constexpr CallablePtr<float64_t(float, float)> FloatIntCallPtr = FloatIntPlus;
	static_assert( FloatIntCallPtr(2.0f, 1.3) == 3.0 );

	IntDecSrc = 5;

	constexpr CallablePtr<uint64_t(int64_t&)> PreIncCallPtr = PreInc;
	Test::testForResult<uint64_t>( "Unary Operator Conversion to CallablePtr", PreIncCallPtr(IntDecSrc), 6);

	constexpr CallablePtr<uint64_t(int64_t&, const int32_t&)> AssignPlusCallPtr = AssignPlusInt;
	Test::testForResult<uint64_t>( "Assign Operator Conversion to CallablePtr", AssignPlusCallPtr(IntDecSrc, 2), 8);
	Test::testForResult<uint64_t>( "Assign Operator Conversion to CallablePtr modifies value as expected.", IntDecSrc, 8);

	#pragma endregion
}