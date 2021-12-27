﻿#include <StdExt/Test/Test.h>

#include <StdExt/Unicode/Iterator.h>
#include <StdExt/Memory.h>
#include <StdExt/String.h>

#include <sstream>

using namespace std;
using namespace StdExt;
using namespace StdExt::Unicode;

using namespace StdExt::Test;

void testString()
{
	constexpr const char* LongString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	constexpr const char* MediumString = "CDEFGHIJKLMNOPQRSTUVWX";
	constexpr const char* SmallString = "GHIJKLMNOPQRST";

	constexpr const char8_t* NonAscii = u8"你好";

	String strLongString(LongString);
	String strMediumString(MediumString);
	String strSmallString(SmallString);

	String litLongString = String::literal(LongString);
	String litMediumString = String::literal(MediumString);
	String litSmallString = String::literal(SmallString);

	String strEmpty;
	String strZeroLength("");

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

	String subStr = strLongString.substr(3, 3);

	Test::testForResult<bool>(
		"subStr() returns correct result with valid parameters.",
		true, subStr == "DEF"
	);
	
	Test::testForResult<bool>(
		"subStr() returns a string stored internally since the length is small.",
		true, subStr.isLocal()
	);

	subStr = strLongString.substr(3, 18);

	Test::testForResult<bool>(
		"subStr() returns correct result with valid parameters.",
		true, subStr == "DEFGHIJKLMNOPQRSTU"
	);

	Test::testForResult<bool>(
		"subStr references same memory as original for long string.",
		true, memory_overlaps(
			(void*)subStr.data(), subStr.length(),
			(void*)strLongString.data(), strLongString.length() )
	);

	Test::testForResult<bool>(
		"subStr is not null terminated since it references part of another string.",
		false, subStr.isNullTerminated()
	);

	String nullTerminated = subStr.getNullTerminated();

	Test::testForResult<bool>(
		"getNullTerminated() returns a null terminated string.",
		true, nullTerminated.isNullTerminated()
	);

	Test::testForResult<bool>(
		"nullTerminated references different memory as original "
		"since it needs to create a new string.",
		true, memory_overlaps(
			(void*)nullTerminated.data(), nullTerminated.length(),
			(void*)strLongString.data(), strLongString.length() )
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