#include <StdExt/Number.h>

#include <StdExt/Test/Test.h>
#include <StdExt/Utility.h>
#include <StdExt/Type.h>

#include <limits>

using namespace StdExt;
using namespace StdExt::Test;

// ---- Number::convert static_assert tests (non-throwing paths) ---------------

// Same type
static_assert(Number::convert<int32_t>(int32_t{42}) == 42);
static_assert(Number::convert<double>(double{1.5}) == 1.5);

// Integer -> Float (all integer values fit)
static_assert(Number::convert<float>(int32_t{5}) == 5.0f);
static_assert(Number::convert<double>(uint64_t{100}) == 100.0);
static_assert(Number::convert<double>(int8_t{-3}) == -3.0);

// Float -> Float: smaller -> larger (no bounds check)
static_assert(Number::convert<double>(float{1.5f}) == static_cast<double>(1.5f));

// Float -> Float: larger -> smaller, in range
static_assert(Number::convert<float>(double{1.5}) == static_cast<float>(1.5));

// Signed -> Signed: smaller -> larger (no bounds check)
static_assert(Number::convert<int64_t>(int8_t{-5}) == -5);
static_assert(Number::convert<int32_t>(int16_t{1000}) == 1000);

// Signed -> Signed: larger -> smaller, in range
static_assert(Number::convert<int8_t>(int32_t{100}) == 100);
static_assert(Number::convert<int16_t>(int64_t{-1000}) == -1000);

// Float -> Integral: in range (truncates toward zero)
static_assert(Number::convert<int32_t>(double{3.7}) == 3);
static_assert(Number::convert<int32_t>(double{-3.7}) == -3);
static_assert(Number::convert<uint8_t>(float{200.9f}) == 200);

// Unsigned -> Unsigned: smaller/equal -> larger (no bounds check)
static_assert(Number::convert<uint64_t>(uint32_t{5}) == 5);
static_assert(Number::convert<uint32_t>(uint8_t{255}) == 255);

// Unsigned -> Unsigned: larger -> smaller, in range
static_assert(Number::convert<uint8_t>(uint32_t{200}) == 200);

// Signed -> Unsigned: non-negative, sizeof(value) <= sizeof(result)
static_assert(Number::convert<uint64_t>(int32_t{5}) == 5);
static_assert(Number::convert<uint32_t>(int8_t{127}) == 127);

// Signed -> Unsigned: non-negative, sizeof(value) > sizeof(result), in range
static_assert(Number::convert<uint8_t>(int32_t{200}) == 200);

// Unsigned -> Signed: sizeof(value) < sizeof(result)
static_assert(Number::convert<int64_t>(uint32_t{5}) == 5);

// Unsigned -> Signed: sizeof(value) >= sizeof(result), in range
static_assert(Number::convert<int8_t>(uint32_t{100}) == 100);
static_assert(Number::convert<int32_t>(uint64_t{1000000}) == 1000000);

// ---- Number::clampConvert static_assert tests (non-throwing paths) ----------

// Same type
static_assert(Number::clampConvert<int32_t>(int32_t{42}) == 42);
static_assert(Number::clampConvert<double>(double{1.5}) == 1.5);

// Integer -> Float
static_assert(Number::clampConvert<float>(int32_t{5}) == 5.0f);
static_assert(Number::clampConvert<double>(uint64_t{100}) == 100.0);
static_assert(Number::clampConvert<double>(int8_t{-3}) == -3.0);

// Float -> Float: smaller -> larger (no bounds check)
static_assert(Number::clampConvert<double>(float{1.5f}) == static_cast<double>(1.5f));

// Float -> Float: larger -> smaller, in range
static_assert(Number::clampConvert<float>(double{1.5}) == static_cast<float>(1.5));

// Float -> Float: larger -> smaller, clamped high
static_assert(Number::clampConvert<float>(double{1e300}) == std::numeric_limits<float>::max());

// Float -> Float: larger -> smaller, clamped low
static_assert(Number::clampConvert<float>(double{-1e300}) == std::numeric_limits<float>::lowest());

// Signed -> Signed: smaller -> larger (no bounds check)
static_assert(Number::clampConvert<int64_t>(int8_t{-5}) == -5);
static_assert(Number::clampConvert<int32_t>(int16_t{1000}) == 1000);

// Signed -> Signed: larger -> smaller, in range
static_assert(Number::clampConvert<int8_t>(int32_t{100}) == 100);

// Signed -> Signed: larger -> smaller, clamped high
static_assert(Number::clampConvert<int8_t>(int32_t{200}) == 127);

// Signed -> Signed: larger -> smaller, clamped low
static_assert(Number::clampConvert<int8_t>(int32_t{-200}) == -128);

// Float -> Integral: in range (truncates toward zero)
static_assert(Number::clampConvert<int32_t>(double{3.7}) == 3);
static_assert(Number::clampConvert<int32_t>(double{-3.7}) == -3);
static_assert(Number::clampConvert<uint8_t>(float{200.9f}) == 200);

// Float -> Integral: clamped high
static_assert(Number::clampConvert<int8_t>(double{1e10}) == 127);

// Float -> Integral: clamped low
static_assert(Number::clampConvert<int8_t>(double{-1e10}) == -128);

// Unsigned -> Unsigned: smaller/equal -> larger
static_assert(Number::clampConvert<uint64_t>(uint32_t{5}) == 5);
static_assert(Number::clampConvert<uint32_t>(uint8_t{255}) == 255);

// Unsigned -> Unsigned: larger -> smaller, in range
static_assert(Number::clampConvert<uint8_t>(uint32_t{200}) == 200);

// Unsigned -> Unsigned: larger -> smaller, clamped
static_assert(Number::clampConvert<uint8_t>(uint32_t{300}) == 255);

// Signed -> Unsigned: positive, sizeof(value) <= sizeof(result)
static_assert(Number::clampConvert<uint64_t>(int32_t{5}) == 5);
static_assert(Number::clampConvert<uint32_t>(int8_t{127}) == 127);

// Signed -> Unsigned: positive, sizeof(value) > sizeof(result), in range
static_assert(Number::clampConvert<uint8_t>(int32_t{200}) == 200);

// Signed -> Unsigned: positive, sizeof(value) > sizeof(result), clamped high
static_assert(Number::clampConvert<uint8_t>(int32_t{300}) == 255);

// Signed -> Unsigned: negative, clamped to 0
static_assert(Number::clampConvert<uint8_t>(int32_t{-5}) == 0);
static_assert(Number::clampConvert<uint32_t>(int64_t{-1}) == 0);

// Unsigned -> Signed: sizeof(value) < sizeof(result)
static_assert(Number::clampConvert<int64_t>(uint32_t{5}) == 5);

// Unsigned -> Signed: sizeof(value) >= sizeof(result), in range
static_assert(Number::clampConvert<int8_t>(uint32_t{100}) == 100);
static_assert(Number::clampConvert<int32_t>(uint64_t{1000000}) == 1000000);

// Unsigned -> Signed: sizeof(value) >= sizeof(result), clamped high
static_assert(Number::clampConvert<int8_t>(uint32_t{200}) == 127);

void testNumber()
{
	Number parsed_num = Number::parse(u8"-3.3");

	testForResult<bool>(
		"String is correctly parsed into a numberic value. (floating point)",
		true, approxEqual(-3.3f, parsed_num.value<float>())
	);

	testForException<std::range_error>(
		"Converting to a type that can't hold the value throws a range exception. (float -> unsigned int)",
		[&]()
		{
			parsed_num.value<uint32_t>();
		}
	);

	testForResult(
		"Interger converion works and rounds as normal conversion does. (-3.3)",
		(int)-3.3, Number(-3.3).value<int>()
	);

	testForResult(
		"Interger converion works and rounds as normal conversion does. (-3.7)",
		(int)-3.7, Number(-3.7).value<int>()
	);

	testForResult(
		"Interger converion works and rounds as normal conversion does. (3.3)",
		(int)3.3, Number(3.3).value<int>()
	);

	testForResult(
		"Interger converion works and rounds as normal conversion does. (3.7)",
		(int)3.7, Number(3.7).value<int>()
	);

	testForResult(
		"Floating point is internally stored as a float64_t.",
		Type<float64_t>::index(), Number(3.7f).storedAsIndex()
	);

	testForResult(
		"Positive integer passed as int is internally stored as a int64_t.",
		Type<int64_t>::index(), Number(3).storedAsIndex()
	);

	testForResult(
		"Positive integer passed as unsigned int is internally stored as a uint64_t.",
		Type<uint64_t>::index(), Number((unsigned int)3).storedAsIndex()
	);

	testForResult(
		"Negative integer is internally stored as a int64_t.",
		Type<int64_t>::index(), Number(-3).storedAsIndex()
	);

	testForResult(
		"Integer below lowest int64_t is stored as a float64_t.",
		Type<float64_t>::index(), Number::parse(u8"-10223372036854775808").storedAsIndex()
	);

	testForResult(
		"Integer above highest uint64_t is stored as a float64_t.",
		Type<float64_t>::index(), Number::parse(u8"19446744073709551615").storedAsIndex()
	);
}