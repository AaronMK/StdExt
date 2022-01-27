#include <StdExt/UnicodeString.h>
#include <StdExt/Unicode/Iterator.h>

#include <locale>

namespace StdExt
{
	using namespace Unicode;
	using namespace std;

	template<>
	UnicodeString<char8_t> fromSysWideChar<char8_t>(std::basic_string_view<wchar_t> str)
	{
		throw not_implemented();
	}

	template<>
	UnicodeString<char16_t> fromSysWideChar<char16_t>(std::basic_string_view<wchar_t> str)
	{
		throw not_implemented();
	}

	template<>
	UnicodeString<char32_t> fromSysWideChar<char32_t>(std::basic_string_view<wchar_t> str)
	{
		throw not_implemented();
	}
}