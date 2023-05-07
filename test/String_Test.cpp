#include <StdExt/Test/Test.h>

#include <StdExt/Unicode/Iterator.h>

#include <StdExt/String.h>
#include <StdExt/Compare.h>

#include <StdExt/Concepts.h>

#include <sstream>

using namespace std;
using namespace StdExt;
using namespace StdExt::Unicode;

using namespace StdExt::Test;

template<Character char_t>
static const char* charName()
{
	if constexpr ( std::same_as<char, char_t> )
		return "char";

	if constexpr ( std::same_as<char8_t, char_t> )
		return "char8_t";
	
	if constexpr ( std::same_as<char16_t, char_t> )
		return "char16_t";
	
	if constexpr ( std::same_as<char32_t, char_t> )
		return "char32_t";
	
	if constexpr ( std::same_as<char, wchar_t> )
		return "wchar_t";

	return "unknown";
};

template<Character from_t, Character to_t, Character in_t>
static auto testConversion(const StringBase<in_t>& input)
{
	auto cvt_in = convertString<from_t>(input);

	stringstream msg;
	msg << "Successfully converts string from " << charName<from_t>() <<
		" to " << charName<to_t>() << " and back.";

	auto cvt_result = 
		convertString<from_t>(
			convertString<to_t>(cvt_in)
		);

	Test::testForResult< StringBase<from_t> >(
		msg.str(), cvt_in, cvt_result
	);
}

void testString()
{
	String CharString = String::literal(u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	constexpr const char8_t* LongString = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	constexpr const char8_t* MediumString = u8"CDEFGHIJKLMNOPQRSTUVWX";
	constexpr const char8_t* SmallString = u8"GHIJKLMNOPQRST";

	constexpr const char8_t* NonAscii = u8"你好";

	U8String strLongString(LongString);
	U8String strMediumString(MediumString);
	U8String strSmallString(SmallString);

	U8String litLongString = U8String::literal(LongString);
	U8String litMediumString = U8String::literal(MediumString);
	U8String litSmallString = U8String::literal(SmallString);
	U8String litNonAscii = U8String::literal(NonAscii);

	U8String strEmpty;
	U8String strZeroLength(u8"");

	U8String complex_conversion =
		litLongString + litNonAscii +
		litMediumString + litNonAscii +
		litSmallString;

	testConversion<char, char8_t>(CharString);
	testConversion<char, char16_t>(CharString);
	testConversion<char, char32_t>(CharString);
	testConversion<char, wchar_t>(CharString);

	testConversion<char8_t, char16_t>(complex_conversion);
	testConversion<char8_t, char32_t>(complex_conversion);
	testConversion<char8_t, wchar_t>(complex_conversion);

	testConversion<char16_t, char8_t>(complex_conversion);
	testConversion<char16_t, char32_t>(complex_conversion);
	testConversion<char16_t, wchar_t>(complex_conversion);

	testConversion<char32_t, char8_t>(complex_conversion);
	testConversion<char32_t, char16_t>(complex_conversion);
	testConversion<char32_t, wchar_t>(complex_conversion);

	testConversion<wchar_t, char8_t>(complex_conversion);
	testConversion<wchar_t, char16_t>(complex_conversion);
	testConversion<wchar_t, char32_t>(complex_conversion);

	try
	{
		testConversion<char8_t, char>(complex_conversion);
		testConversion<char16_t, char>(complex_conversion);
		testConversion<char32_t, char>(complex_conversion);
		testConversion<wchar_t, char>(complex_conversion);
	}
	catch(const std::exception&)
	{
		cout << "Note: wide/UTF conversion to narrow character strings"
			" is limited on this platform." << std::endl;
	}

	Test::testForResult<bool>(
		"strLongString initially null-terminated.",
		true, strLongString.isNullTerminated()
		);

	Test::testForResult<bool>(
		"strSmallString initially null-terminated.",
		true, strSmallString.isNullTerminated()
		);

	Test::testForResult<bool>(
		"strLongString is not local",
		false, strLongString.isLocal()
		);

	Test::testForResult<bool>(
		"strLongString is stored on the heap.",
		true, strLongString.isOnHeap()
		);

	Test::testForResult<bool>(
		"strSmallString is local",
		true, strSmallString.isLocal()
		);

	Test::testForResult<bool>(
		"strSmallString is not stored on the heap.",
		false, strSmallString.isOnHeap()
		);

	Test::testForResult<bool>(
		"litLongString is not stored on the heap.",
		false, litLongString.isOnHeap()
		);

	Test::testForResult<bool>(
		"litSmallString is not stored on the heap.",
		false, litSmallString.isOnHeap()
		);

	Test::testForResult<bool>(
		"litLongString is not local.",
		false, litLongString.isLocal()
		);

	Test::testForResult<bool>(
		"litSmallString is not local.",
		false, litSmallString.isLocal()
		);

	U8String subStr = strLongString.substr(3, 3);

	Test::testForResult<int>(
		"subStr() returns correct result with valid parameters.",
		0, subStr == std::u8string_view(u8"DEF")
		);

	Test::testForResult<bool>(
		"subStr() returns a string stored internally since the length is small.",
		true, subStr.isLocal()
		);

	subStr = strLongString.substr(3, 18);

	Test::testForResult<int>(
		"subStr() returns correct result with valid parameters.",
		0, subStr == std::u8string_view(u8"DEFGHIJKLMNOPQRSTU")
		);

	Test::testForResult<bool>(
		"subStr references same memory as original for long string.",
		true, memory_overlaps(
			subStr.data(), subStr.size(),
			strLongString.data(), strLongString.size())
		);

	Test::testForResult<bool>(
		"subStr is not null terminated since it references part of another string.",
		false, subStr.isNullTerminated()
		);

	U8String nullTerminated = subStr.getNullTerminated();

	Test::testForResult<bool>(
		"getNullTerminated() returns a null terminated string.",
		true, nullTerminated.isNullTerminated()
		);

	Test::testForResult<bool>(
		"nullTerminated references different memory as original "
		"since it needs to create a new string.",
		false, memory_overlaps(
			nullTerminated.data(), nullTerminated.size(),
			strLongString.data(), strLongString.size())
		);

	subStr = strLongString.substr(3, 23);

	Test::testForResult<bool>(
		"subStr null terminated since it goes to the end of the original string.",
		true, subStr.isNullTerminated()
		);

	nullTerminated = subStr.getNullTerminated();

	Test::testForResult<bool>(
		"nullTerminated references same memory as original "
		"since original was also null-terminated.",
		true, nullTerminated.data() == subStr.data()
		);
}