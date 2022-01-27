#ifndef _STD_EXT_UNICODE_STRING_H_
#define _STD_EXT_UNICODE_STRING_H_

#include "StdExt.h"
#include "Concepts.h"

#include "Collections/SharedArray.h"
#include "Collections/Collections.h"

#include "Unicode/Iterator.h"

#include <string>

namespace StdExt
{
	/**
	 * @brief
	 *  %String class that avoids deep copying by sharing data among copies and substrings,
	 *  and limits its character types to unicode for greater interoperablity.
	 */
	template<UnicodeCharacter char_t>
	class UnicodeString
	{
	public:
		using iterator_t = Unicode::CodePointIterator<char_t>;
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
		static constexpr size_t npos = std::basic_string_view<char_t>::npos;

		static UnicodeString join(const UnicodeString* strings, size_t count, view_t glue);
		static UnicodeString join(const std::vector<UnicodeString>& strings, view_t glue);

		///@{
		/**
		 * @brief
		 *  Creates a %String object that will either copy the literal string data into
		 *  its small memory, or wrap functionality around the referenced data.  Either
		 *  way, this avoids a heap allocation.  The behavior of the created string object
		 *  becomes undefined if it outlives the string data.
		 */
		static UnicodeString literal(const view_t& str) noexcept;
		static UnicodeString literal(const char_t* str) noexcept;
		static UnicodeString literal(const char_t* str, size_t char_count) noexcept;
		///@}

		UnicodeString() noexcept;
		UnicodeString(const view_t& str);
		UnicodeString(const char_t* str);
		UnicodeString(UnicodeString&& other) noexcept;
		UnicodeString(const UnicodeString& other) noexcept;
		UnicodeString(const shared_array_t& other) noexcept;

		UnicodeString& operator=(const view_t& other) noexcept;
		UnicodeString& operator=(const char_t* str) noexcept;
		UnicodeString& operator=(UnicodeString&& other) noexcept;
		UnicodeString& operator=(const UnicodeString& other) noexcept;

		std::strong_ordering operator<=>(const view_t& other) const;
		std::strong_ordering operator<=>(const char_t* other) const;
		std::strong_ordering operator<=>(const UnicodeString& other) const;

		UnicodeString operator+(const view_t& other) const;
		UnicodeString operator+(const char_t* other) const;
		UnicodeString operator+(const UnicodeString& other) const;

		void operator+=(const view_t& other);
		void operator+=(const char_t* other);
		void operator+=(const UnicodeString& other);

		int compare(const view_t& other) const;
		int compare(const char_t* other) const;
		int compare(const UnicodeString& other) const;

		const char_t* findFirstOf(const view_t& str);
		const char_t* findFirstOf(const char_t* str);
		const char_t* findFirstOf(const UnicodeString& str);

		const char_t* findFirstNotOf(const view_t& str);
		const char_t* findFirstNotOf(const char_t* str);
		const char_t* findFirstNotOf(const UnicodeString& str);

		const char_t* findLastOf(const view_t& str);
		const char_t* findLastOf(const char_t* str);
		const char_t* findLastOf(const UnicodeString& str);

		const char_t* findLastNotOf(const view_t& str);
		const char_t* findLastNotOf(const char_t* str);
		const char_t* findLastNotOf(const UnicodeString& str);

		const char_t* data() const;
		size_t size() const;

		/**
		 * @brief
		 *  Returns true if the data of the string is externally managed.  This is
		 *  the case with string constructed using UnicodeString::literal().
		 */
		bool isExternal() const;

		/**
		 * @brief
		 *  Returns true if the data of the string is in shared memory on the heap
		 *  managed by this and potentially other string objects.
		 */
		bool isOnHeap() const;

		/**
		 * @brief
		 *  Returns true if the data of the string is stored inside the memory of
		 *  the string object itself.
		 */
		bool isInternal() const;

		/**
		 * @brief
		 *  Returns true if the string is null. This means there is no string, not
		 *  even an empty one.
		 */
		bool isNull() const;

	private:

		void moveFrom(UnicodeString&& other) noexcept;

		void copyFrom(const UnicodeString& other) noexcept;
		void copyFrom(const view_t& view);

		/**
		 * @internal
		 * @brief
		 *  A view of the data representing the string.  This will be null if the string
		 *  is empty.  For small strings, it will reference the relevent portion of
		 *  mSmallMemory.  If large, it will reference the relevent portion of 
		 *  mHeapReference.  If literal, it will be a view of the string data passed
		 *  to the constructor.
		 */
		view_t                               mView;

		/**
		 * @internal
		 * @brief
		 *  References heap memory that stores the string.  This will be shared with
		 *  other %String objects that are copies or created a sub-string
		 *  of this string.  It is null when the string is null, stored in
		 *  mSmallMemory, or is literal.
		 */
		shared_array_t                       mHeapReference;

		/**
		 * @internal
		 * @brief
		 *  Storage for small strings to reduce heap access and small allocations.  This
		 *  will always be null-terminated, and is only used when mView points to this
		 *  data.
		 */
		std::array<char_t, SmallSize + 1>  mSmallMemory;

	};

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> fromSysWideChar(std::basic_string_view<wchar_t> str);

	template<>
	STD_EXT_EXPORT UnicodeString<char8_t> fromSysWideChar<char8_t>(std::basic_string_view<wchar_t> str);

	template<>
	STD_EXT_EXPORT UnicodeString<char16_t> fromSysWideChar<char16_t>(std::basic_string_view<wchar_t> str);

	template<>
	STD_EXT_EXPORT UnicodeString<char32_t> fromSysWideChar<char32_t>(std::basic_string_view<wchar_t> str);

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::join(const UnicodeString* strings, size_t count, view_t glue)
	{
		size_t length = 0;

		for (size_t i = 0; i < count; ++i)
			length += strings[i].size();

		length += (count - 1) * glue.length();

		shared_array_t memory(length);

		char_t* start = memory.data();
		size_t index = 0;

		for (size_t i = 0; i < count; ++i)
		{
			auto source = strings[i].mView;
			Collections::copy_n(source.data(), &start[index], source.size());
			index += strings[i].size();

			if (i != count - 1)
			{
				glue.copy(&start[index], glue.length());
				index += glue.length();
			}
		}

		start[length] = '\0';

		return UnicodeString(std::move(memory));
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::join(const std::vector<UnicodeString>& strings, view_t glue)
	{
		return join(&strings[0], strings.size(), glue);
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::literal(const char_t* str) noexcept
	{
		UnicodeString ret;
		ret.mView = view_t(str);

		return ret;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::literal(const char_t* str, size_t length) noexcept
	{
		UnicodeString ret;
		ret.mView = view_t(str, length);

		return ret;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::literal(const view_t& str) noexcept
	{
		UnicodeString ret;
		ret.mView = view_t(str);

		return ret;
	}

	template<UnicodeCharacter char_t>
	StdExt::UnicodeString<char_t>::UnicodeString() noexcept
	{
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const view_t& str)
		: UnicodeString()
	{
		copyFrom(str);
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const char_t* str)
		: UnicodeString(view_t(str))
	{
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(UnicodeString&& other) noexcept
	{
		moveFrom(std::move(other));
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const shared_array_t& other) noexcept
	{
		assert( 0 == other[other.size() - 1] );
		
		if (other.size() <= SmallSize)
		{
			Collections::copy_n(other.data(), mSmallMemory.data(), other.size());
			mView = view_t(mSmallMemory.data());
		}
		else
		{
			mHeapReference = other;
			mView = view_t(mHeapReference.data(), mHeapReference.size());
		}
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const UnicodeString& other) noexcept
	{
		copyFrom(other);
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>& UnicodeString<char_t>::operator=(const view_t& other) noexcept
	{
		copyFrom(other);
		return *this;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>& UnicodeString<char_t>::operator=(const char_t* str) noexcept
	{
		copyFrom(view_t(str));
		return *this;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>& UnicodeString<char_t>::operator=(UnicodeString&& other) noexcept
	{
		moveFrom(std::move(other));
		return *this;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>& UnicodeString<char_t>::operator=(const UnicodeString& other) noexcept
	{
		copyFrom(other);
		return *this;
	}


	template<UnicodeCharacter char_t>
	std::strong_ordering UnicodeString<char_t>::operator<=>(const view_t& other) const
	{
		return mView <=> other;
	}

	template<UnicodeCharacter char_t>
	std::strong_ordering UnicodeString<char_t>::operator<=>(const char_t* other) const
	{
		return mView <=> view_t(other);
	}
	
	template<UnicodeCharacter char_t>
	std::strong_ordering UnicodeString<char_t>::operator<=>(const UnicodeString& other) const
	{
		return mView <=> other.mView;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::operator+(const view_t& other) const
	{
		if (other.size() == 0)
			return *this;

		size_t combinedSize = mView.size() + other.size();
		char_t* outMemory = nullptr;
		UnicodeString<char_t> ret;

		if (combinedSize <= SmallSize)
		{
			outMemory = ret.mSmallMemory.data();
		}
		else
		{
			ret.mHeapReference = shared_array_t(combinedSize + 1);
			outMemory = ret.mHeapReference.data();
		}

		memcpy(outMemory, data(), size() * sizeof(char_t));
		memcpy(outMemory + size(), other.data(), other.size());
		outMemory[combinedSize] = '\0';

		ret.mView = view_t(outMemory, combinedSize);

		return ret;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::operator+(const char_t* other) const
	{
		return *this + view_t(other);
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::operator+(const UnicodeString& other) const
	{
		return *this + other.mView;
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::operator+=(const view_t& other)
	{
		if (other.size() == 0)
			return;

		size_t combinedSize = size() + other.size();

		if (combinedSize <= SmallSize)
		{
			memcpy(&mSmallMemory[size()], other.data(), other.size());
			mView = view_t(&mSmallMemory[0], combinedSize);
		}
		else
		{
			shared_array_t memory(combinedSize + 1);

			memcpy(memory.data(), data(), size() * sizeof(char_t));
			memcpy(memory.data() + size(), other.data(), other.size() * sizeof(char_t));

			mHeapReference = memory;
			mView = view_t(mHeapReference.data(), combinedSize);
		}
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::operator+=(const char_t* other)
	{
		*this += view_t(other);
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::operator+=(const UnicodeString& other)
	{
		*this += other.mView;
	}

	template<UnicodeCharacter char_t>
	int UnicodeString<char_t>::compare(const view_t& other) const
	{
		return mView.compare(other);
	}

	template<UnicodeCharacter char_t>
	int UnicodeString<char_t>::compare(const char_t* other) const
	{
		return mView.compare(other);
	}

	template<UnicodeCharacter char_t>
	inline int UnicodeString<char_t>::compare(const UnicodeString& other) const
	{
		return mView.compare(other.mView);
	}

	template<UnicodeCharacter char_t>
	const char_t* UnicodeString<char_t>::data() const
	{
		return mView.data();
	}

	template<UnicodeCharacter char_t>
	size_t UnicodeString<char_t>::size() const
	{
		return mView.size();
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::isExternal() const
	{
		return ( 
			nullptr != mView.data() &&
			mHeapReference.isNull() &&
			false == memory_ecompases(
				&mSmallMemory[0], &mSmallMemory[mSmallMemory.size() - 1],
				&mView[0], &mView[mView.size() - 1]
			)
		);
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::isOnHeap() const
	{
		return (!mHeapReference.isNull());
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::isInternal() const
	{
		return 
			nullptr != mView.data() &&
			memory_ecompases(
				&mSmallMemory[0], &mSmallMemory[mSmallMemory.size() - 1],
				&mView[0], &mView[mView.size() - 1]
			);
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::isNull() const
	{
		return (nullptr == mView.data());
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::moveFrom(UnicodeString&& other) noexcept
	{
		if (other.mHeapReference)
		{
			mHeapReference = std::move(other.mHeapReference);
			mSmallMemory[0] = 0;
			mView = other.mView;
		}
		else
		{
			mHeapReference.makeNull();
			Collections::copy_n(other.mSmallMemory.data(), mSmallMemory.data(), mSmallMemory.size());
			mView = view_t(&mSmallMemory[0], other.mView.size());
		}

		other.mSmallMemory[0] = 0;
		other.mView = view_t();
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::copyFrom(const UnicodeString& other) noexcept
	{
		if ( other.isInternal() )
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

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::copyFrom(const view_t& view)
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
}

#endif // !_STD_EXT_UNICODE_STRING_H_