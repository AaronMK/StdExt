#include <StdExt/Test/Test.h>
#include <StdExt/Unicode/Iterator.h>

#include <StdExt/UnicodeString.h>
#include <StdExt/Any.h>

#include <sstream>

using namespace std;
using namespace StdExt;
using namespace StdExt::Unicode;

using namespace StdExt::Test;

template<UnicodeCharacter char_t>
static int StrCmp(const char_t* left, const char_t* right)
{
	basic_string_view<char_t> vL(left);
	basic_string_view<char_t> vR(right);

	return vL.compare(vR);
}

template<UnicodeCharacter char_t>
void testIterator()
{
	auto result = StrCmp(u8"A😊C", u8"A😊CDEF");

	if constexpr ( std::is_same_v<char_t, char8_t> )
	{
		const char8_t* chars[] = {
			u8"B",
			u8"Ώ",
			u8"A",
			u8"お",
			u8"名",
			u8"😊",
			u8"前"
		};

		CodePointIterator<char8_t> itr(u8"BΏAお名😊前");

		size_t i = 0;
		do
		{
			stringstream msg;
			msg << "CodePointIterator<char8_t> forward chars[" << i << "].";

			Test::testForResult<int>(
				msg.str(),
				0 , StrCmp<char8_t>(*itr, chars[i])
			);

			itr.next();
			++i;
		}
		while ( itr );

		Test::testForResult<const char8_t*>(
			"CodePointIterator<char8_t> null at end.",
			nullptr , *itr
		);

		do
		{
			itr.previous();
			--i;

			stringstream msg;
			msg << "CodePointIterator<char8_t> reverse chars[" << i << "].";

			Test::testForResult<int>(
				msg.str(),
				0 , StrCmp<char8_t>(*itr, chars[i])
			);
		}
		while ( i > 0 );
	}
	else if constexpr ( std::is_same_v<char_t, char16_t> )
	{
		const char16_t* chars[] = {
			u"B",
			u"Ώ",
			u"A",
			u"お",
			u"名",
			u"😊",
			u"前"
		};

		CodePointIterator<char16_t> itr(u"BΏAお名😊前");

		size_t i = 0;
		do
		{
			stringstream msg;
			msg << "CodePointIterator<char16_t> forward chars[" << i << "].";

			Test::testForResult<int>(
				msg.str(),
				0 , StrCmp(*itr, chars[i])
			);

			itr.next();
			++i;
		}
		while ( itr );

		Test::testForResult<const char16_t*>(
			"CodePointIterator<char16_t> null at end.",
			nullptr , *itr
		);

		do
		{
			itr.previous();
			--i;

			stringstream msg;
			msg << "CodePointIterator<char16_t> reverse chars[" << i << "].";

			Test::testForResult<int>(
				msg.str(),
				0 , StrCmp(*itr, chars[i])
			);
		}
		while ( i > 0 );
	}
	else if constexpr ( std::is_same_v<char_t, char32_t> )
	{
		const char32_t* chars[] = {
			U"B",
			U"Ώ",
			U"A",
			U"お",
			U"名",
			U"😊",
			U"前"
		};

		CodePointIterator<char32_t> itr(U"BΏAお名😊前");

		size_t i = 0;
		do
		{
			stringstream msg;
			msg << "CodePointIterator<char32_t> forward chars[" << i << "].";

			Test::testForResult<int>(
				msg.str(),
				0 , StrCmp(*itr, chars[i])
			);

			itr.next();
			++i;
		}
		while ( itr );

		Test::testForResult<const char32_t*>(
			"CodePointIterator<char32_t> null at end.",
			nullptr , *itr
		);

		do
		{
			itr.previous();
			--i;

			stringstream msg;
			msg << "CodePointIterator<char32_t> reverse chars[" << i << "].";

			Test::testForResult<int>(
				msg.str(),
				0 , StrCmp(*itr, chars[i])
			);
		}
		while ( i > 0 );
	}
}

template<UnicodeCharacter char_t>
void testString()
{
	using ustring_t = UnicodeString<char_t>;
	
	ustring_t LiteralString;
	ustring_t LongString;
	ustring_t ShortString;
	
	{
		Any anyLiteralString;
		Any anyLongString;
		Any anyShortString;

		if constexpr (std::is_same_v<char_t, char8_t>)
		{
			anyLiteralString.setValue<ustring_t>(ustring_t::literal(u8"BΏAお名😊前BΏAお名😊前BΏAお名😊前"));
			anyLongString.setValue<ustring_t>(u8"前BΏAお名😊前");
			anyShortString.setValue<ustring_t>(u8"お名");

		}
		else if constexpr (std::is_same_v<char_t, char16_t>)
		{
			anyLiteralString.setValue<ustring_t>(ustring_t::literal(u"BΏAお名😊前BΏAお名😊前BΏAお名😊前"));
			anyLongString.setValue<ustring_t>(u"前BΏAお名😊前");
			anyShortString.setValue<ustring_t>(u"お名");
		}
		else if constexpr (std::is_same_v<char_t, char32_t>)
		{
			anyLiteralString.setValue<ustring_t>(ustring_t::literal(U"BΏAお名😊前BΏAお名😊前BΏAお名😊前"));
			anyLongString.setValue<ustring_t>(U"前BΏAお名😊前");
			anyShortString.setValue<ustring_t>(U"お名");
		}

		LiteralString = *anyLiteralString.cast<ustring_t>();
		LongString = *anyLongString.cast<ustring_t>();
		ShortString = *anyShortString.cast<ustring_t>();
	}

	Test::testForResult<bool>(
		"Literal string is external",
		true, LiteralString.isExternal()
	);

	Test::testForResult<bool>(
		"Literal string is not on heap",
		false, LiteralString.isOnHeap()
	);

	Test::testForResult<bool>(
		"Literal string is not internal",
		false, LiteralString.isInternal()
	);

	Test::testForResult<bool>(
		"Long string is not external",
		false, LongString.isExternal()
	);

	Test::testForResult<bool>(
		"Long string is on heap",
		true, LongString.isOnHeap()
	);

	Test::testForResult<bool>(
		"Long string is not internal",
		false, LongString.isInternal()
	);

	Test::testForResult<bool>(
		"Short string is not external",
		false, ShortString.isExternal()
	);

	Test::testForResult<bool>(
		"Short string is not on heap",
		false, ShortString.isOnHeap()
	);

	Test::testForResult<bool>(
		"Short string is internal",
		true, ShortString.isInternal()
	);

	auto compare_result = LongString <=> ShortString;
}

void testUnicode()
{
	testString<char8_t>();
	testString<char16_t>();
	testString<char32_t>();

	testIterator<char8_t>();
	testIterator<char16_t>();
	testIterator<char32_t>();
}