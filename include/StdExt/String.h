#ifndef _STD_EXT_STRING_H_
#define _STD_EXT_STRING_H_

#include "StdExt.h"
#include "Concepts.h"

#include "Collections/SharedArray.h"
#include "Const/String.h"
#include "Memory/Utility.h"
#include "Serialize/Binary/Binary.h"
#include "Streams/ByteStream.h"

#include <string>
#include <array>
#include <span>

namespace StdExt
{
	/**
	 * @brief
	 *  Returns a view of the string with trailing zero terminators omitted.
	 */
	template<Character char_t>
	static std::basic_string_view<char_t> trimEnd(const std::basic_string_view<char_t>& view)
	{
		size_t view_size = view.size();
		if (nullptr == view.data() )
			return view;
		
		while ( view_size > 0 && 0 == view[view_size - 1] )
			--view_size;

		return std::basic_string_view<char_t>(view.data(), view_size);
	}

	/**
	 * @brief
	 *  %String class that avoids deep copying by sharing data among copies and substrings,
	 *  and limits its character types to unicode for greater interoperablity.
	 */
	template<Character char_t>
	class StringBase
	{
	public:
		using view_t = std::basic_string_view<char_t>;
		using shared_array_t = Collections::SharedArray<char_t>;

		static constexpr size_t SmallByteSize = 16;
		static_assert(
			SmallByteSize % 4 == 0 && SmallByteSize > 1,
			"SmallSize must be a multiple of 4 bytes and greater than 1."
		);

		/**
		 * @brief
		 *  The maximum size in bytes for which seperate memory allocation will not occur.
		 *  Strings at or below this size are stored directly in the string object.
		 */
		static constexpr size_t SmallSize = SmallByteSize / sizeof(char_t);

		/**
		 * @brief
		 *  Constant value to indicate _no position_.  It is returned by some functions
		 *  when a string is not found.
		 */
		static constexpr size_t npos = view_t::npos;

		static StringBase join(std::span<const StringBase> strings, StringBase glue = StringBase())
		{
			size_t length = 0;
			size_t count = strings.size();

			for (size_t i = 0; i < count; ++i)
				length += strings[i].size();

			length += (count - 1) * glue.size();

			shared_array_t memory(length + 1);
			memory[length] = 0;

			char_t* start = memory.data();
			size_t index = 0;

			for (size_t i = 0; i < count; ++i)
			{
				auto source = strings[i].mView;
				Collections::copy_n(source.data(), &start[index], source.size());
				index += strings[i].size();

				if (i != count - 1 && glue.size() > 0)
				{
					Collections::copy_n(glue.data(), &start[index], glue.size());
					index += glue.size();
				}
			}

			return StringBase(std::move(memory));
		}

		///@{
		/**
		 * @brief
		 *  Creates a %String object that will either copy the literal string data into
		 *  its small memory, or wrap functionality around the referenced data.  Either
		 *  way, this avoids a heap allocation.  The behavior of the created string object
		 *  becomes undefined if it outlives the string data.
		 */
		static StringBase literal(const char_t* str) noexcept
		{
			StringBase ret;
			ret.mView = view_t(str);

			return ret;
		}

		static StringBase literal(const view_t& str) noexcept
		{
			StringBase ret;
			ret.mView = trimEnd(str);

			return ret;
		}

		static StringBase literal(const char_t* str, size_t char_count) noexcept
		{
			StringBase ret;
			ret.mView = trimEnd(view_t(str, char_count));

			return ret;
		}
		///@}

		StringBase() noexcept
		{
			mSmallMemory[SmallSize] = 0;
		}

		template<size_t N>
		StringBase(Const::String<char_t, N> str) noexcept
			: StringBase()
		{
			mView = str.view();
		}

		StringBase(const char_t* str)
			: StringBase(view_t(str))
		{
		}

		StringBase(const char_t* str, size_t length)
			: StringBase( view_t(str, length) )
		{
		}

		StringBase(view_t str)
			: StringBase()
		{
			copyFrom( trimEnd(str) );
		}

		StringBase(StringBase&& other) noexcept
			: StringBase()
		{
			moveFrom(std::move(other));
		}

		StringBase(const StringBase& other) noexcept
			: StringBase()
		{
			copyFrom(other);
		}

		StringBase(shared_array_t&& other) noexcept
			: StringBase()
		{
			assert(0 == other[other.size() - 1]);

			if (other.size() <= SmallSize)
			{
				Collections::copy_n(other.data(), mSmallMemory.data(), other.size());
				mView = view_t(mSmallMemory.data(), other.size() - 1);
				mSmallMemory[other.size()] = 0;
			}
			else
			{
				mHeapReference = std::move(other);
				mView = view_t(mHeapReference.data(), mHeapReference.size() - 1);
			}
		}

		StringBase(const shared_array_t& other) noexcept
			: StringBase(std::move(shared_array_t(other)))
		{
		}

		StringBase& operator=(const view_t& other) noexcept
		{
			copyFrom(other);
			return *this;
		}

		StringBase& operator=(StringBase&& other) noexcept
		{
			moveFrom(std::move(other));
			return *this;
		}

		StringBase& operator=(const StringBase& other) noexcept
		{
			copyFrom(other);
			return *this;
		}

		auto operator<=>(const view_t& other) const noexcept
		{
			return mView <=> other;
		}

		auto operator<=>(const StringBase& other) const noexcept
		{
			return mView <=> other.mView;
		}

		bool operator==(const view_t& other) const noexcept
		{
			return (size() == other.size() && 0 == mView.compare(other));
		}

		bool operator==(const StringBase& other) const noexcept
		{
			return (size() == other.size() && 0 == mView.compare(other.mView));
		}

		bool operator!=(const view_t& other) const noexcept
		{
			return (size() != other.size() || 0 != mView.compare(other));
		}

		bool operator!=(const StringBase& other) const noexcept
		{
			return (size() != other.size() || 0 != mView.compare(other.mView));
		}

		StringBase operator+(const view_t& other) const
		{
			if (other.size() == 0)
				return *this;

			size_t combinedSize = mView.size() + other.size();
			char_t* outMemory = nullptr;
			StringBase<char_t> ret;

			if (combinedSize <= SmallSize)
			{
				outMemory = ret.mSmallMemory.data();
			}
			else
			{
				ret.mHeapReference = shared_array_t(combinedSize + 1);
				outMemory = ret.mHeapReference.data();
				outMemory[combinedSize] = 0;
			}

			Collections::copy_n(data(), outMemory, size());
			Collections::copy_n(other.data(), outMemory + size(), other.size());

			ret.mView = view_t(outMemory, combinedSize);

			return ret;
		}

		const char_t& operator[](size_t index)
		{
			return mView[index];
		}

		StringBase operator+(const char_t* other) const
		{
			return *this + view_t(other);
		}

		StringBase operator+(const StringBase& other) const
		{
			return *this + other.mView;
		}

		void operator+=(const char_t* other)
		{
			*this += view_t(other);
		}

		void operator+=(const view_t& other)
		{
			if (other.size() == 0)
				return;

			size_t combinedSize = size() + other.size();

			if (combinedSize <= SmallSize)
			{
				Collections::copy_n(other.data(), &mSmallMemory[size()], other.size());
				mView = view_t(&mSmallMemory[0], combinedSize);
				mSmallMemory[combinedSize] = 0;
			}
			else
			{
				shared_array_t memory(combinedSize + 1);
				memory[combinedSize] = 0;

				Collections::copy_n(data(), memory.data(), size());
				Collections::copy_n(other.data(), memory.data() + size(), other.size());

				mHeapReference = memory;
				mView = view_t(mHeapReference.data(), combinedSize);
			}
		}

		void operator+=(const StringBase& other)
		{
			*this += other.mView;
		}

		size_t find(const view_t& str, size_t pos = 0) const
		{
			return mView.find(str, pos);
		}

		size_t find(const StringBase& str, size_t pos = 0) const
		{
			return mView.find(str.mView, pos);
		}

		size_t findFirstOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_first_of(str, pos);
		}

		size_t findFirstOf(const StringBase& str, size_t pos = 0) const
		{
			return mView.find_first_of(str.mView, pos);
		}

		size_t findFirstNotOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_first_not_of(str, pos);
		}

		size_t findFirstNotOf(const StringBase& str, size_t pos = 0) const
		{
			return mView.find_first_not_of(str.mView, pos);
		}

		size_t findLastOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_last_of(str, pos);
		}

		size_t findLastOf(const StringBase& str, size_t pos = 0) const
		{
			return mView.find_last_of(str.mView, pos);
		}

		size_t findLastNotOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_last_not_of(str, pos);
		}

		size_t findLastNotOf(const StringBase& str, size_t pos = 0) const
		{
			return mView.find_last_not_of(str.mView, pos);
		}

		StringBase substr(size_t pos, size_t count = npos) const
		{
			view_t subView = mView.substr(pos, count);

			if (subView.size() <= SmallSize)
			{
				return StringBase(subView);
			}
			else if (isExternal())
			{
				StringBase ret;
				ret.mView = subView;

				return ret;
			}
			else
			{
				StringBase ret;
				ret.mView = subView;
				ret.mHeapReference = mHeapReference;

				return ret;
			}
		}

		std::vector<StringBase> split(const view_t& deliminator, bool keepEmpty = true) const
		{
			std::vector<StringBase> ret;

			size_t strSize = size();
			size_t delimSize = deliminator.size();
			size_t begin = 0;
			size_t end = 0;

			while (begin < strSize && end != npos)
			{
				end = find(deliminator, begin);

				if (end != npos)
				{
					if (keepEmpty || end != begin)
						ret.push_back(substr(begin, end - begin));

					begin = end + delimSize;
				}
			}

			if (begin < strSize)
				ret.emplace_back(substr(begin, strSize - begin));
			else if (begin == strSize && keepEmpty)
				ret.emplace_back(StringBase());

			return ret;
		}

		std::vector<StringBase> split(const StringBase& deliminator, bool keepEmpty = true) const
		{
			return split(deliminator.mView, keepEmpty);
		}

		std::vector<StringBase> split(const char_t* deliminator, bool keepEmpty = true) const
		{
			return split(view_t(deliminator), keepEmpty);
		}

		/**
		 * @brief
		 *  Returns true if data() represents a null-terminated charater string of the full
		 *  string and can be safely used as such.
		 *
		 *  For strings with external data, this may be true, but can't be determined by
		 *  the class itself.  For data managed by the class, this can be determined.  All
		 *  that data will eventually be null terminated, but if the object is not
		 *  referencing the full set of shared data, it may not null-terminate at the
		 *  end of the string of this object.
		 */
		bool isNullTerminated() const
		{
			if (isExternal())
				return false;

			const char_t* addrNullCheck = mView.data() + mView.size();
			return *addrNullCheck == '\0';
		}

		/**
		 * @brief
		 *  Gets a string object where data() will returns a null-terminated string equivelent
		 *  to the original string.
		 */
		StringBase getNullTerminated() const
		{
			if (isNullTerminated())
				return *this;
			else
				return StringBase(mView);
		}

		const char_t* data() const
		{
			return mView.data();
		}

		size_t size() const
		{
			return mView.size();
		}

		const view_t view() const
		{
			return mView;
		}

		/**
		 * @brief
		 *  Returns true if the data of the string is externally managed.  This is
		 *  the case with string constructed using StringBase::literal().
		 */
		bool isExternal() const
		{
			return (
				nullptr != mView.data() &&
				mHeapReference.isNull() &&
				false == memory_ecompases<const char_t>(
					std::span<const char_t>(mSmallMemory.data(), mSmallMemory.size()),
					std::span<const char_t>(mView.data(), mView.size())
					)
				);
		}

		/**
		 * @brief
		 *  Returns true if the data of the string is in shared memory on the heap
		 *  managed by this and potentially other string objects.
		 */
		bool isOnHeap() const
		{
			return (!mHeapReference.isNull());
		}

		/**
		 * @brief
		 *  Returns true if the data of the string is stored inside the memory of
		 *  the string object itself.
		 */
		bool isLocal() const
		{
			return
				nullptr != mView.data() &&
				memory_ecompases<const char_t>(
					std::span<const char_t>(mSmallMemory.data(), mSmallMemory.size()),
					std::span<const char_t>(mView.data(), mView.size())
					);
		}

		/**
		 * @brief
		 *  Returns true if the string is null. This means there is no string, not
		 *  even an empty one.
		 */
		bool isNull() const
		{
			return (nullptr == mView.data());
		}

		std::basic_string<char_t> toStdString() const
		{
			return std::basic_string<char_t>(mView);
		}

		operator view_t() const
		{
			return mView;
		}

	private:

		void moveFrom(StringBase&& other) noexcept
		{
			if (other.isLocal())
			{
				mHeapReference.makeNull();
				Collections::copy_n(other.mSmallMemory.data(), mSmallMemory.data(), mSmallMemory.size());
				mView = view_t(&mSmallMemory[0], other.mView.size());
			}
			else
			{
				mHeapReference = std::move(other.mHeapReference);
				mSmallMemory[0] = 0;
				mView = other.mView;
			}

			other.mSmallMemory[0] = 0;
			other.mView = view_t();
		}

		void copyFrom(const StringBase& other) noexcept
		{
			if (other.isLocal())
			{
				Collections::copy_n(other.mSmallMemory.data(), mSmallMemory.data(), mSmallMemory.size());
				mView = view_t(mSmallMemory.data(), other.size());

				mHeapReference.makeNull();
			}
			else
			{
				mView = other.mView;
				mHeapReference = other.mHeapReference;
				mSmallMemory[0] = 0;
			}
		}

		void copyFrom(const view_t& view)
		{
			if (view.size() <= SmallSize)
			{
				mHeapReference.makeNull();
				Collections::copy_n(view.data(), mSmallMemory.data(), view.size());
				mSmallMemory[view.size()] = 0;

				mView = view_t(mSmallMemory.data(), view.size());
			}
			else
			{
				mHeapReference = shared_array_t(view.size() + 1);
				Collections::copy_n(view.data(), mHeapReference.data(), view.size());
				mHeapReference[view.size()] = 0;

				mView = view_t(mHeapReference.data(), view.size());
			}
		}

		/**
		 * @internal
		 * @brief
		 *  A view of the data representing the string.  This will be null if the string
		 *  is empty.  For small strings, it will reference the relevent portion of
		 *  mSmallMemory.  If large, it will reference the relevent portion of
		 *  mHeapReference.  If literal, it will be a view of the string data passed
		 *  to the constructor.
		 */
		view_t                             mView;

		/**
		 * @internal
		 * @brief
		 *  References heap memory that stores the string.  This will be shared with
		 *  other %String objects that are copies or created a sub-string
		 *  of this string.  It is null when the string is null, stored in
		 *  mSmallMemory, or is literal.
		 */
		shared_array_t                     mHeapReference;

		/**
		 * @internal
		 * @brief
		 *  Storage for small strings to reduce heap access and small allocations.  This
		 *  will always be null-terminated, and is only used when mView points to this
		 *  data.
		 */
		std::array<char_t, SmallSize + 1>  mSmallMemory;
	};

	using CString = StringBase<char>;
	using U8String = StringBase<char8_t>;
	using U16String = StringBase<char16_t>;
	using U32String = StringBase<char32_t>;
	using WString = StringBase<wchar_t>;

	using String = U8String;

	template<Character to_t, Character from_t>
	StringBase<to_t> convertString(const StringBase<from_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char> convertString<char>(const StringBase<char>& str);

	template<>
	STD_EXT_EXPORT StringBase<char> convertString<char>(const StringBase<char8_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char> convertString<char>(const StringBase<char16_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char> convertString<char>(const StringBase<char32_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char> convertString<char>(const StringBase<wchar_t>& str);



	template<>
	STD_EXT_EXPORT StringBase<char8_t> convertString<char8_t>(const StringBase<char>& str);

	template<>
	STD_EXT_EXPORT StringBase<char8_t> convertString<char8_t>(const StringBase<char8_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char8_t> convertString<char8_t>(const StringBase<char16_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char8_t> convertString<char8_t>(const StringBase<char32_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char8_t> convertString<char8_t>(const StringBase<wchar_t>& str);



	template<>
	STD_EXT_EXPORT StringBase<char16_t> convertString<char16_t>(const StringBase<char>& str);

	template<>
	STD_EXT_EXPORT StringBase<char16_t> convertString<char16_t>(const StringBase<char8_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char16_t> convertString<char16_t>(const StringBase<char16_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char16_t> convertString<char16_t>(const StringBase<char32_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char16_t> convertString<char16_t>(const StringBase<wchar_t>& str);



	template<>
	STD_EXT_EXPORT StringBase<char32_t> convertString<char32_t>(const StringBase<char>& str);

	template<>
	STD_EXT_EXPORT StringBase<char32_t> convertString<char32_t>(const StringBase<char8_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char32_t> convertString<char32_t>(const StringBase<char16_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char32_t> convertString<char32_t>(const StringBase<char32_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<char32_t> convertString<char32_t>(const StringBase<wchar_t>& str);



	template<>
	STD_EXT_EXPORT StringBase<wchar_t> convertString<wchar_t>(const StringBase<char>& str);

	template<>
	STD_EXT_EXPORT StringBase<wchar_t> convertString<wchar_t>(const StringBase<char8_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<wchar_t> convertString<wchar_t>(const StringBase<char16_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<wchar_t> convertString<wchar_t>(const StringBase<char32_t>& str);

	template<>
	STD_EXT_EXPORT StringBase<wchar_t> convertString<wchar_t>(const StringBase<wchar_t>& str);

	template<Character to_t, Character from_t>
	StringBase<to_t> convertString(std::basic_string<from_t> view)
	{
		return convertString<to_t>( StringBase<from_t>::literal(view) );
	}

	template<Character to_t, Character from_t>
	StringBase<to_t> convertString(std::basic_string_view<from_t> view)
	{
		return convertString<to_t>( StringBase<from_t>::literal(view) );
	}
}

template<StdExt::Character char_t>
StdExt::StringBase<char_t> operator+(typename StdExt::StringBase<char_t>::view_t left, const StdExt::StringBase<char_t>& right)
{
	using view_t = typename StdExt::StringBase<char_t>::view_t;

	size_t combined_size = left.size() + right.size();
	char_t* out_buffer = nullptr;

	auto writeCombined = [&](char_t* out)
	{
		StdExt::Collections::copy_n(left.data(), out, left.size());
		StdExt::Collections::copy_n(right.data(), &out[left.size()], right.size());
		out[combined_size] = 0;
	};

	if (combined_size > StdExt::StringBase<char_t>::SmallSize)
	{
		typename StdExt::StringBase<char_t>::shared_array_t string_data(combined_size + 1);
		writeCombined(string_data.data());

		return StdExt::StringBase<char_t>(std::move(string_data));
	}
	else
	{
		std::array<char_t, StdExt::StringBase<char_t>::SmallSize + 1> string_data;
		writeCombined(string_data.data());

		return StdExt::StringBase<char_t>(
			view_t(string_data.data(), string_data.size() - 1)
			);
	}
}

template<StdExt::Character char_t>
StdExt::StringBase<char_t> operator+(const char_t* left, const StdExt::StringBase<char_t>& right)
{
	return StdExt::StringBase<char_t>::view_t(left) + right;
}

namespace StdExt::Serialize::Binary
{
	template<>
	STD_EXT_EXPORT void read<StdExt::CString>(StdExt::Streams::ByteStream* stream, StdExt::CString* out);

	template<>
	STD_EXT_EXPORT void write<StdExt::CString>(StdExt::Streams::ByteStream* stream, const StdExt::CString& val);

	template<>
	STD_EXT_EXPORT void read<StdExt::U8String>(StdExt::Streams::ByteStream* stream, StdExt::U8String* out);

	template<>
	STD_EXT_EXPORT void write<StdExt::U8String>(StdExt::Streams::ByteStream* stream, const StdExt::U8String& val);

	template<>
	STD_EXT_EXPORT void read<StdExt::U16String>(StdExt::Streams::ByteStream* stream, StdExt::U16String* out);

	template<>
	STD_EXT_EXPORT void write<StdExt::U16String>(StdExt::Streams::ByteStream* stream, const StdExt::U16String& val);

	template<>
	STD_EXT_EXPORT void read<StdExt::U32String>(StdExt::Streams::ByteStream* stream, StdExt::U32String* out);

	template<>
	STD_EXT_EXPORT void write<StdExt::U32String>(StdExt::Streams::ByteStream* stream, const StdExt::U32String& val);
}

#endif // !_STD_EXT_STRING_H_