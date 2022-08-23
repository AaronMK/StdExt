#include <StdExt/Number.h>

#include <StdExt/Test/Test.h>
#include <StdExt/Utility.h>
#include <StdExt/Type.h>

using namespace StdExt;
using namespace StdExt::Test;

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