#ifndef _STD_EXT_UNICODE_ITERATOR_H_
#define _STD_EXT_UNICODE_ITERATOR_H_

#include "../Concepts.h"

#include <string_view>
#include <array>

namespace StdExt::Unicode
{
	class unicode_format_error : public std::exception
	{
	public:
		unicode_format_error() {};

		virtual const char* what() const noexcept override
		{
			return "Unicode format error.";
		}
	};

	template<UnicodeCharacter char_t>
	class CodePointIterator
	{
	private:

		static constexpr size_t max_unit_size = 4/sizeof(char_t);
		using view_t = std::basic_string_view<char_t>;

		/**
		 * @internal
		 * @brief
		 *  Zero-filled when at the end of the string view.
		 */
		std::array<char_t, max_unit_size + 1> mCodePointData;

		/**
		 * @internal
		 * @brief
		 *  The full string view over which this object is iterating.
		 */
		view_t mFullView;

		/**
		 * @internal
		 * @brief
		 *  The length of the current code point in code units.
		 */
		size_t mLength;
		
		/**
		 * @internal
		 *  The index of the code unit in mFullView at which
		 *  mCodePointData starts.  If this is greater than or equal to
		 *  the size of mFullView, it means the iterator is at the end.
		 */
		size_t mIndex;

		/**
		 * @internal
		 * @brief
		 *  Loads the data for the codepoint at the current mIndex of mFullView
		 *  into mCodePointData, and stores the length in units in mLength.
		 *  If loading fails, mLength and mCodePointData will remain unchanged.
		 * 
		 * @returns
		 *  True if the end of the string or a valid code point is at mIndex and
		 *  can be loaded, false if there is a format error preventing this.
		 */
		bool loadCodePointData();

	public:
		CodePointIterator(const view_t& view);

		/**
		 * @brief
		 *  Attempts to move the iterator to the next code point or to the end.
		 *  The will return true if not already at the end of the string.  If
		 *  formating errors are found, a unicode_format_error will be thrown.
		 */
		bool next();

		bool previous();

		void moveToEnd();

		/**
		 * @brief
		 *  Gets a pointer to a null-terminated sequence of code units
		 *  representing the current code point.
		 */
		const char_t* operator*() const noexcept;

		/**
		 * @brief
		 *  Gets the length in terms of code units of the current character.
		 */
		size_t length() const noexcept;

		operator bool() const;
	};

	template<UnicodeCharacter char_t>
	CodePointIterator<char_t>::CodePointIterator(const view_t& view)
		: mFullView(view)
	{
		mIndex = 0;
		mLength = 0;

		loadCodePointData();
	}
	
	template<UnicodeCharacter char_t>
	bool CodePointIterator<char_t>::loadCodePointData()
	{
		if ( mIndex >= mFullView.size() )
		{
			mCodePointData[0] = 0;
			mLength = 0;

			return true;
		}
		else
		{
			const char_t* str = &mFullView[mIndex];
			size_t num_units = 0;

			if constexpr ( std::is_same_v<char_t, char8_t> )
			{
				if ( str[0] >> 7 == 0 )
					num_units = 1;
				else if ( str[0] >> 5 == 0x06 )
					num_units = 2;
				else if ( str[0] >> 4 == 0x0E )
					num_units = 3;
				else if ( str[0] >> 3 == 0x1E )
					num_units = 4;
				else
					return false;

				if ( mFullView.size() < mIndex + num_units )
					return false;

				for ( size_t i = 1; i < num_units; ++i )
				{
					if ( str[i] >> 6 != 0x2 )
						return false;
				}
			}
			else if constexpr ( std::is_same_v<char_t, char16_t> )
			{
				if ( str[0] < 0xD800 || str[0] > 0xE000 )
					num_units = 1;
				else if ( str[0] >= 0xD800 && str[0] < 0xDC00 )
					num_units = 2;
				else
					return false;

				if ( mFullView.size() < mIndex + num_units )
					return false;

				if ( 2 == num_units && ( str[1] < 0xDC00 || str[1] > 0xDFFF ) )
					return false;
			}
			else // char32_t
			{
				num_units = 1;

				if ( mFullView.size() < mIndex + num_units )
					return false;
			}

			for ( size_t i = 0; i < num_units; ++i )
				mCodePointData[i] = str[i];

			mCodePointData[num_units] = 0;
			mLength = num_units;

			return true;
		}
	}
	
	template<UnicodeCharacter char_t>
	bool CodePointIterator<char_t>::next()
	{
		if ( mIndex >= mFullView.size() )
			return false;
		
		size_t old_index = mIndex;
		mIndex += mLength;

		if ( !loadCodePointData() )
		{
			mIndex = old_index;
			throw unicode_format_error();
		}

		return true;
	}
	
	template<UnicodeCharacter char_t>
	bool CodePointIterator<char_t>::previous()
	{
		if ( 0 == mIndex )
			return false;

		size_t init_index = mIndex;
		size_t last_index = (init_index > max_unit_size ) ?
		                    (init_index - max_unit_size) : 0;

		for ( size_t i = init_index - 1; i >= last_index; --i )
		{
			mIndex = i;
			if ( loadCodePointData() )
				return true;
		}

		mIndex = init_index;
		throw unicode_format_error();
	}
	
	template<UnicodeCharacter char_t>
	void CodePointIterator<char_t>::moveToEnd()
	{
		mCodePointData[0] = 0;
		mIndex = mFullView.size();
		mLength = 0;
	}
	
	template<UnicodeCharacter char_t>
	const char_t* CodePointIterator<char_t>::operator*() const noexcept
	{
		return ( mLength > 0 ) ? &mCodePointData[0] : nullptr;
	}
	
	template<UnicodeCharacter char_t>
	size_t CodePointIterator<char_t>::length() const noexcept
	{
		return mLength;
	}
	
	template<UnicodeCharacter char_t>
	CodePointIterator<char_t>::operator bool() const
	{
		return ( 0 != mCodePointData[0] );
	}
}

#endif // !_STD_EXT_UNICODE_ITERATOR_H_
