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
		static constexpr size_t npos = view_t::npos;

		static UnicodeString join(const std::span<UnicodeString>& strings, UnicodeString glue)
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

				if (i != count - 1)
				{
					Collections::copy_n(glue.data(), &start[index], glue.size());
					index += glue.size();
				}
			}

			return UnicodeString(std::move(memory));
		}

		///@{
		/**
		 * @brief
		 *  Creates a %String object that will either copy the literal string data into
		 *  its small memory, or wrap functionality around the referenced data.  Either
		 *  way, this avoids a heap allocation.  The behavior of the created string object
		 *  becomes undefined if it outlives the string data.
		 */
		static UnicodeString literal(const char_t* str) noexcept
		{
			UnicodeString ret;
			ret.mView = view_t(str);

			return ret;
		}

		static UnicodeString literal(const view_t& str) noexcept
		{
			UnicodeString ret;
			ret.mView = view_t(str);

			return ret;
		}

		static UnicodeString literal(const char_t* str, size_t char_count) noexcept
		{
			UnicodeString ret;
			ret.mView = view_t(str, char_count);

			return ret;
		}
		///@}

		UnicodeString() noexcept
		{
			mSmallMemory[SmallSize] = 0;
		}

		UnicodeString(const char_t* str)
			: UnicodeString(view_t(str))
		{
		}

		UnicodeString(const view_t& str)
			: UnicodeString()
		{
			copyFrom(str);
		}

		UnicodeString(UnicodeString&& other) noexcept
			: UnicodeString()
		{
			moveFrom(std::move(other));
		}

		UnicodeString(const UnicodeString& other) noexcept
			: UnicodeString()
		{
			copyFrom(other);
		}

		UnicodeString(const shared_array_t& other) noexcept
			: UnicodeString()
		{
			assert( 0 == other[other.size() - 1] );

			if (other.size() <= SmallSize)
			{
				Collections::copy_n(other.data(), mSmallMemory.data(), other.size());
				mView = view_t(mSmallMemory.data(), other.size());
				mSmallMemory[other.size()] = 0;
			}
			else
			{
				mHeapReference = other;
				mView = view_t(mHeapReference.data(), mHeapReference.size() - 1);
			}
		}

		UnicodeString& operator=(const view_t& other) noexcept
		{
			copyFrom(other);
			return *this;
		}

		UnicodeString& operator=(UnicodeString&& other) noexcept
		{
			moveFrom(std::move(other));
			return *this;
		}

		UnicodeString& operator=(const UnicodeString& other) noexcept
		{
			copyFrom(other);
			return *this;
		}

		std::strong_ordering operator<=>(const char_t* other) const
		{
			return mView <=> view_t(other);
		}

		std::strong_ordering operator<=>(const view_t& other) const
		{
			return mView <=> other;
		}

		std::strong_ordering operator<=>(const UnicodeString& other) const
		{
			return mView <=> other.mView;
		}

		bool operator==(const view_t& other) const
		{
			return (0 == mView.compare(other));
		}

		bool operator==(const UnicodeString& other) const
		{
			return (0 == mView.compare(other.mView));
		}

		bool operator!=(const view_t& other) const
		{
			return (0 != mView.compare(other));
		}

		bool operator!=(const UnicodeString& other) const
		{
			return (0 != mView.compare(other.mView));
		}

		UnicodeString operator+(const view_t& other) const
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
				outMemory[combinedSize] = 0;
			}

			Collections::copy_n(data(), outMemory, size());
			Collections::copy_n(other.data(), outMemory + size(), other.size());

			ret.mView = view_t(outMemory, combinedSize);

			return ret;
		}

		UnicodeString operator+(const char_t* other) const
		{
			return *this + view_t(other);
		}

		UnicodeString operator+(const UnicodeString& other) const
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

		void operator+=(const UnicodeString& other)
		{
			*this += other.mView;
		}

		size_t find(const view_t& str, size_t pos = 0) const
		{
			return mView.find(str, pos);
		}

		size_t find(const UnicodeString& str, size_t pos = 0) const
		{
			return mView.find(str.mView, pos);
		}

		size_t findFirstOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_first_of(str, pos);
		}

		size_t findFirstOf(const UnicodeString& str, size_t pos = 0) const
		{
			return mView.find_first_of(str.mView, pos);
		}

		size_t findFirstNotOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_first_not_of(str, pos);
		}

		size_t findFirstNotOf(const UnicodeString& str, size_t pos = 0) const
		{
			return mView.find_first_not_of(str.mView, pos);
		}

		size_t findLastOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_last_of(str, pos);
		}

		size_t findLastOf(const UnicodeString& str, size_t pos = 0) const
		{
			return mView.find_last_of(str.mView, pos);
		}

		size_t findLastNotOf(const view_t& str, size_t pos = 0) const
		{
			return mView.find_last_not_of(str, pos);
		}

		size_t findLastNotOf(const UnicodeString& str, size_t pos = 0) const
		{
			return mView.find_last_not_of(str.mView, pos);
		}

		UnicodeString substr(size_t pos, size_t count = npos) const
		{
			view_t subView = mView.substr(pos, count);
			
			if (subView.size() <= SmallSize)
			{
				return UnicodeString(subView);
			}
			else if ( isExternal() )
			{
				UnicodeString ret;
				ret.mView = subView;

				return ret;
			}
			else
			{
				UnicodeString ret;
				ret.mView = subView;
				ret.mHeapReference = mHeapReference;

				return ret;
			}
		}

		std::vector<UnicodeString> split(const view_t& deliminator, bool keepEmpty = true) const
		{
			std::vector<UnicodeString> ret;

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
				ret.emplace_back(UnicodeString());

			return ret;
		}

		std::vector<UnicodeString> split(const UnicodeString& deliminator, bool keepEmpty = true) const
		{
			return split(deliminator.mView, keepEmpty);
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
			if ( isExternal() )
				return false;

			const char_t* addrNullCheck = mView.data() + mView.size();
			return *addrNullCheck == '\0';
		}

		/**
		 * @brief
		 *  Gets a string object where data() will returns a null-terminated string equivelent
		 *  to the original string.
		 */
		UnicodeString getNullTerminated() const
		{
			if (isNullTerminated())
				return *this;
			else
				return UnicodeString(mView);
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
		 *  the case with string constructed using UnicodeString::literal().
		 */
		bool isExternal() const
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
				memory_ecompases(
					&mSmallMemory[0], &mSmallMemory[mSmallMemory.size() - 1],
					&mView[0], &mView[mView.size() - 1]
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

	private:

		void moveFrom(UnicodeString&& other) noexcept
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

		void copyFrom(const UnicodeString& other) noexcept
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
}

#endif // !_STD_EXT_UNICODE_STRING_H_