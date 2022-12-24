#ifndef _STD_EXT_CONST_STRING_H_
#define _STD_EXT_CONST_STRING_H_

#include "../Concepts.h"

#include <string>
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

		constexpr const char* data() const
		{
			return chars.data();
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

#if 0
	namespace detail
	{
		template<Character char_t, size_t str_size>
		class BaseString
		{
		public:
			using view_t = std::basic_string_view<char_t, str_size>;
			const view_t mStr
		
			consteval BaseString(view_t str)
		};
	}

	



	template<Character char_t, size_t t_size>
	class StringBase
	{
	};


	template<Character char_t>
	class String
	{
	public:
		using view_t = std::basic_string_view<char_t>;

		const char* mChars;

		consteval String(const char_t* str)
			: mChars(str)
		{
		}

		consteval size_t size() const
		{
			return strLength(mChars);
		}

		consteval const char_t* data() const
		{
			return mChars;
		}

		consteval bool operator==(const String& other) const
		{
			if ( size() != other.size() )
				return false;

			return (0 == strCompare(mChars, other.mChars));
		}

		consteval std::strong_ordering operator<=>(const String& other) const
		{
			return view_t(mChars) <=> view_t(other.mChars);
		}
	};
#endif
}

#endif // !_STD_EXT_CONST_STRING_H_


