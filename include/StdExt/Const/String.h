#ifndef _STD_EXT_CONST_STRING_H_
#define _STD_EXT_CONST_STRING_H_

#include "../Concepts.h"

#include <array>

namespace StdExt::Const
{
	template<Character char_t>
	constexpr size_t strLength(const char_t* str)
	{
		return std::basic_string_view<char_t>(str).size();
	}

	template<Character char_t>
	constexpr int strCompare(
		std::basic_string_view<char_t> left,
		std::basic_string_view<char_t> right)
	{
		return left.compare(right);
	}

	template<Character char_t, size_t N>
	class String
	{
	public:
		using array_t = std::array<char_t, N>;
		
		const array_t chars;

		consteval String(const char_t (&str)[N])
			: chars( make(str) )
		{
		}

		constexpr size_t size() const
		{
			return strLength(chars.data());
		}

		constexpr const char_t* data() const
		{
			return chars.data();
		}

		constexpr std::basic_string_view<char_t> view() const
		{
			return std::basic_string_view<char_t>(data(), size());
		}

		constexpr friend bool operator==(String, String) = default;
	
	private:
		static constexpr array_t make(const char_t (&str)[N])
		{
			array_t result;

			for ( int i = 0; i < N; ++i )
				result[i] = str[i];

			return result;
		}
	};
}

#endif // !_STD_EXT_CONST_STRING_H_


