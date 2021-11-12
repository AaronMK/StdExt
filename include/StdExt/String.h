#ifndef _STD_EXT_STRING_H_
#define _STD_EXT_STRING_H_

#include "Memory.h"
#include "Concepts.h"
#include "Exceptions.h"

#include <array>
#include <vector>
#include <variant>
#include <string>
#include <iostream>

#include "Unicode/Iterator.h"

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

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
		
		using Iterator = Unicode::CodePointIterator<char_t>;

		using view_t = std::basic_string_view<char_t>;

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

		UnicodeString();
		UnicodeString(const view_t& str);
		UnicodeString(const char_t* str);
		UnicodeString(UnicodeString&& other);
		UnicodeString(const UnicodeString& other);
		
		UnicodeString& operator=(const view_t& other) const;
		UnicodeString& operator=(const char_t* str) noexcept;
		UnicodeString& operator=(UnicodeString&& other) noexcept;
		UnicodeString& operator=(const UnicodeString& other) noexcept;

		bool operator<(const view_t& other) const;
		bool operator<(const char_t* other) const;
		bool operator<(const UnicodeString& other) const;

		bool operator<=(const view_t& other) const;
		bool operator<=(const char_t* other) const;
		bool operator<=(const UnicodeString& other) const;

		bool operator==(const view_t& other) const;
		bool operator==(const char_t* other) const;
		bool operator==(const UnicodeString& other) const;

		bool operator!=(const view_t& other) const;
		bool operator!=(const char_t* other) const;
		bool operator!=(const UnicodeString& other) const;

		bool operator>=(const view_t& other) const;
		bool operator>=(const char_t* other) const;
		bool operator>=(const UnicodeString& other) const;

		bool operator>(const view_t& other) const;
		bool operator>(const char_t* other) const;
		bool operator>(const UnicodeString& other) const;

		UnicodeString operator+(const view_t& other) const;
		UnicodeString operator+(const char_t* other) const;
		UnicodeString operator+(const UnicodeString& other) const;

		void operator+=(const view_t& other) const;
		void operator+=(const char_t* other) const;
		void operator+=(const UnicodeString& other) const;

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

	private:

		bool isExternal() const;
		bool isOnHeap() const;
		bool isNull() const;

		view_t                               mView;

		/**
		 * @internal
		 * @brief
		 *  References heap memory that stores the string.  This will be shared with
		 *  other %String objects that are copies or created a sub-string
		 *  of this string.  It is null when the string is null, stored in
		 *  mSmallMemory, or is literal.
		 */
		MemoryReference                    mHeapReference;

		/**
		 * @internal
		 * @brief
		 *  Storage for small strings to reduce heap access and small allocations.  This
		 *  will always be null-terminated, and is only used when mView points to this
		 *  data.
		 */
		std::array<char_t, SmallSize + 1>  mSmallMemory;

		void moveFrom(UnicodeString&& other);

		void copyFrom(const UnicodeString& other);
		void copyFrom(const view_t& view);
	};

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::join(const UnicodeString* strings, size_t count, view_t glue)
	{
		throw not_implemented();
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::join(const std::vector<UnicodeString>& strings, view_t glue)
	{
		return join(&strings[0], strings.size(), glue);
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::literal(const char_t* str) noexcept
	{
		return literal(view_t(str));
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::literal(const char_t* str, size_t length) noexcept
	{
		throw not_implemented();
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::literal(const view_t& str) noexcept
	{
		
		if ( str.size() <= SmallSize )
		{
			return UnicodeString(str);
		}
		else
		{
			UnicodeString ret;
			ret.mView = str;

			return ret;
		}
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString()
	{
		mSmallMemory[0] = 0;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const view_t& str)
		: UnicodeString()
	{
		copyFrom(str);
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const char_t* str)
		: UnicodeString( view_t(str) )
	{
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(UnicodeString&& other)
	{
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>::UnicodeString(const UnicodeString& other)
	{
		throw not_implemented();
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>& UnicodeString<char_t>::operator=(const view_t& other) const
	{
		copyFrom(other);
		return *this;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t>& UnicodeString<char_t>::operator=(const char_t* str) noexcept
	{
		copyFrom(view_t(other));
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
	bool UnicodeString<char_t>::operator<(const view_t& other) const
	{
		return mView.compare(other) < 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator<(const char_t* other) const
	{
		return mView.compare(other) < 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator<(const UnicodeString& other) const
	{
		return mView.compare(other.mView) < 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator<=(const view_t& other) const
	{
		return mView.compare(other) <= 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator<=(const char_t* other) const
	{
		return mView.compare(other) <= 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator<=(const UnicodeString& other) const
	{
		return mView.compare(other.mView) <= 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator==(const view_t& other) const
	{
		return mView.compare(other) == 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator==(const char_t* other) const
	{
		return mView.compare(other) == 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator==(const UnicodeString& other) const
	{
		return mView.compare(other.mView) == 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator!=(const view_t& other) const
	{
		return mView.compare(other) != 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator!=(const char_t* other) const
	{
		return mView.compare(other) != 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator!=(const UnicodeString& other) const
	{
		return mView.compare(other.mView) != 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator>=(const view_t& other) const
	{
		return mView.compare(other) >= 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator>=(const char_t* other) const
	{
		return mView.compare(other) >= 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator>=(const UnicodeString& other) const
	{
		return mView.compare(other.mView) >= 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator>(const view_t& other) const
	{
		return mView.compare(other) > 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator>(const char_t* other) const
	{
		return mView.compare(other) > 0;
	}

	template<UnicodeCharacter char_t>
	bool UnicodeString<char_t>::operator>(const UnicodeString& other) const
	{
		return mView.compare(other.mView) > 0;
	}

	template<UnicodeCharacter char_t>
	UnicodeString<char_t> UnicodeString<char_t>::operator+(const view_t& other) const
	{
		if ( other.size() == 0 )
			return *this;

		size_t combinedSize = mView.size() + other.size();
		char* outMemory = nullptr;
		UnicodeString<char_t> ret;

		if (combinedSize <= SmallSize)
		{
			outMemory = ret.mSmallMemory.data();
		}
		else
		{
			ret.mHeapReference = MemoryReference(combinedSize + 1);
			outMemory = (char_t*)ret.mHeapReference.data();
		}

		memcpy(outMemory, data(), size());
		memcpy(outMemory + size(), other.data(), other.size());
		outMemory[combinedSize] = '\0';
		
		ret.mView = std::string_view(outMemory, combinedSize);

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
	void UnicodeString<char_t>::operator+=(const view_t& other) const
	{
		if ( other.size() == 0 )
			return;

		size_t combinedSize = size() + other.size();

		if (combinedSize <= SmallSize)
		{
			memcpy(&mSmallMemory[size()], other.data(), other.size());
			mView = std::string_view(&mSmallMemory[0], combinedSize);
		}
		else
		{
			MemoryReference memory(combinedSize + 1);

			memcpy(memory.data(), data(), size());
			memcpy((char*)memory.data() + size(), other.data(), other.size());

			mHeapReference = memory;
			mView = std::string_view((char*)mHeapReference.data(), combinedSize);
		}
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::operator+=(const char_t* other) const
	{
		*this += view_t(other);
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::operator+=(const UnicodeString& other) const
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
	bool UnicodeString<char_t>::isExternal() const
	{
		return ( nullptr != mView.data() &&
		         mHeapReference.isNull() &&
		         0 == mSmallMemory[0]       );
	}

	template<UnicodeCharacter char_t>
	inline bool UnicodeString<char_t>::isOnHeap() const
	{
		return ( !mHeapReference.isNull() );
	}

	template<UnicodeCharacter char_t>
	inline bool UnicodeString<char_t>::isNull() const
	{
		return (nullptr == mView.data() );
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::moveFrom(UnicodeString&& other)
	{
		if ( other.mHeapReference )
		{
			mHeapReference = std::move(other.mHeapReference);
			mSmallMemory[0] = 0;
			mView = other.mView;
		}
		else
		{
			mHeapReference.makeNull();
			mSmallMemory = other.mSmallMemory;
			mView = std::basic_string_view<char_t>(&mSmallMemory[0], other.mView.size());
		}

		other.mSmallMemory[0] = 0;
		other.mView = std::basic_string_view<char_t>();
	}

	template<UnicodeCharacter char_t>
	void UnicodeString<char_t>::copyFrom(const UnicodeString& other)
	{
		if ( 0 != mSmallMemory[0] )
		{
			mSmallMemory = other.mSmallMemory;
			mView = view_t(mSmallMemory.data(), other.mSmallMemory.size());

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
		if ( view.size() <= SmallSize )
		{
			mHeapReference.makeNull();
			memcpy(mSmallMemory.data(), view.data(), view.size() * sizeof(char_t));
			mSmallMemory[view.size()] = 0;

			mView = view_t( mSmallMemory.data(), view.size());
		}
		else
		{
			mHeapReference = MemoryReference((view.size() + 1) * sizeof(char_t), alignof(char_t));
			char_t* heapBegin = access_as<char_t*>(mHeapReference.data());

			memcpy(heapBegin, view.data(), view.size() * sizeof(char_t));
			heapBegin[view.size()] = 0;

			mView = view_t(heapBegin, view.size());
		}
	}

	/**
	 * @brief
	 *  %String class that avoids deep copying required by std::string by sharing data.
	 */
	class STD_EXT_EXPORT String
	{
		friend class StringHelper;

	public:

		/**
		 * @brief
		 *  The maximum string length for which a seperate memory allocation will not occur.
		 *  Strings at or below this length are stored directly in the string object.
		 */
		static constexpr size_t SmallSize = 14;

		/**
		 * @brief
		 *  Constant value to indicate _no position_.  It is returned by some functions
		 *  when a string is not found.
		 */
		static constexpr size_t npos { std::basic_string_view<char8_t>::npos };

		static String join(const String* strings, size_t count, std::string_view glue);
		static String join(const std::vector<String>& strings, std::string_view glue);

		static String literal(const char* str) noexcept;
		static String literal(const char* str, size_t length) noexcept;
		static String literal(const std::string_view& str) noexcept;

		constexpr String() noexcept;

		String(const char* str);
		String(const char* str, size_t size);
		String(String&& other) noexcept;
		String(const String& other) noexcept;
		String(const std::string_view& str);

		String(MemoryReference&& mem) noexcept;
		String(const MemoryReference& mem) noexcept;

		explicit String(const std::string& stdStr);
		explicit String(std::string&& stdStr);

		/**
		 * @brief
		 *  Convertion to a std::string.  This will never be done implicitly
		 *  to avoid unexcepected memory allocations.
		 */
		std::string toStdString() const;
		
		String& operator=(String&& other) noexcept;
		String& operator=(const String& other);
		String& operator=(const std::string& stdStr);
		String& operator=(std::string&& stdStr);
		String& operator=(const char* str);

		bool operator==(const char* other) const;
		bool operator==(const String& other) const;
		bool operator==(const std::string_view& other) const;

		bool operator!=(const char* other) const;
		bool operator!=(const String& other) const;
		bool operator!=(const std::string_view& other) const;

		bool operator<(const char* other) const;
		bool operator<(const String& other) const;
		bool operator<(const std::string_view& other) const;

		bool operator>(const char* other) const;
		bool operator>(const String& other) const;
		bool operator>(const std::string_view& other) const;

		bool operator<=(const char* other) const;
		bool operator<=(const String& other) const;
		bool operator<=(const std::string_view& other) const;

		bool operator>=(const char* other) const;
		bool operator>=(const String& other) const;
		bool operator>=(const std::string_view& other) const;

		char operator[](size_t index) const;

		String operator+(const char* other) const;
		String operator+(const String& other) const;
		String operator+(const std::string& other) const;
		String operator+(const std::string_view& other) const;

		String& operator+=(const char* other);
		String& operator+=(const String& other);
		String& operator+=(const std::string& other);
		String& operator+=(const std::string_view& other);

		int compare(std::string_view other) const;
		int compare(const char* other) const;

		size_t length() const;
		size_t size() const;

		size_t copy(char* dest, size_t count, size_t pos = 0) const;

		String substr(size_t pos, size_t count = npos) const;

		size_t find(const String& str, size_t pos = 0) const;
		size_t find(std::string_view v, size_t pos = 0) const;
		size_t find(char c, size_t pos = 0) const;
		size_t find(const char* c, size_t pos, size_t count) const;
		size_t find(const char* c, size_t pos = 0) const;

		size_t rfind(const String& str, size_t pos = 0) const;
		size_t rfind(std::string_view v, size_t pos = 0) const;
		size_t rfind(char c, size_t pos = 0) const;
		size_t rfind(const char* c, size_t pos, size_t count) const;
		size_t rfind(const char* c, size_t pos = 0) const;

		size_t find_first_of(const String& str, size_t pos = 0) const;
		size_t find_first_of(std::string_view v, size_t pos = 0) const;
		size_t find_first_of(char c, size_t pos = 0) const;
		size_t find_first_of(const char* c, size_t pos, size_t count) const;
		size_t find_first_of(const char* c, size_t pos = 0) const;

		size_t find_last_of(const String& str, size_t pos = npos) const;
		size_t find_last_of(std::string_view v, size_t pos = npos) const;
		size_t find_last_of(char c, size_t pos = npos) const;
		size_t find_last_of(const char* c, size_t pos, size_t count) const;
		size_t find_last_of(const char* c, size_t pos = npos) const;

		size_t find_first_not_of(const String& str, size_t pos = 0) const;
		size_t find_first_not_of(std::string_view v, size_t pos = 0) const;
		size_t find_first_not_of(char c, size_t pos = 0) const;
		size_t find_first_not_of(const char* c, size_t pos, size_t count) const;
		size_t find_first_not_of(const char* c, size_t pos = 0) const;

		size_t find_last_not_of(const String& str, size_t pos = npos) const;
		size_t find_last_not_of(std::string_view v, size_t pos = npos) const;
		size_t find_last_not_of(char c, size_t pos = npos) const;
		size_t find_last_not_of(const char* c, size_t pos, size_t count) const;
		size_t find_last_not_of(const char* c, size_t pos = npos) const;

		std::vector<String> split(const String& str, bool keepEmpty = true) const;
		std::vector<String> split(std::string_view deliminator, bool keepEmpty = true) const;
		std::vector<String> split(char deliminator, bool keepEmpty = true) const;
		std::vector<String> split(const char* c, size_t pos, size_t count, bool keepEmpty = true) const;
		std::vector<String> split(const char* c, size_t pos = 0, bool keepEmpty = true) const;

		String trim();

		/**
		 * @brief
		 *  Returns true if the internal storage of the string is null-terminated.  If true, the
		 *  character pointer returned by data() can be passed directly into c-style functions.
		 */
		bool isNullTerminated() const;

		/**
		 * @brief
		 *  Returns a %String for which data() will return a null-terminated c-style string and
		 *  isNullTerminated() will be true.
		 */
		String getNullTerminated() const;

		/**
		 * @brief
		 *  Returns a raw character pointer to the string data.  This is not guarenteed to be
		 *  null terminiated.
		 *
		 * @see
		 *  getNullTerminated()
		 */
		const char* data() const;

		/**
		 * @brief
		 *  Returns a std::string_view of the contained string.
		 */
		std::string_view view() const;

		/**
		 * @brief
		 *  Implicit conversion to std::string_view of the data contained
		 *  within the string.
		 */
		operator std::string_view() const;

		/**
		 * @brief
		 *  If one string is a substring of the other, that string will reference the
		 *  sub-section of the larger string.
		 */
		static void consolidate(const String& left, const String& right);

		/**
		 * @brief
		 *  Returns true if the string data is stored in the small memory of the
		 *  string object itself.
		 */
		bool isLocal() const;

		/**
		 * @brief
		 *  Returns true if the string data is stored in a heap allocation
		 *  outside the string object.
		 */
		bool isOnHeap() const;

	private:
		constexpr String(bool external, const std::string_view& str) noexcept
			: mView(str), mIsLiteral(external)
		{
		}

		std::string_view                 mView;
		MemoryReference                  mHeapReference;
		std::array<char, SmallSize + 1>  mSmallMemory;
		bool                             mIsLiteral;

		void moveFrom(String&& other);

		void copyFrom(const String& other);
		void copyFrom(const std::string_view& view);
	};
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

STD_EXT_EXPORT std::ostream& operator<<(std::ostream& stream, const StdExt::String& str);

STD_EXT_EXPORT StdExt::String operator+(const char* left, const StdExt::String& right);
STD_EXT_EXPORT StdExt::String operator+(const std::string& left, const StdExt::String& right);
STD_EXT_EXPORT StdExt::String operator+(const std::string_view& left, const StdExt::String& right);

STD_EXT_EXPORT bool operator<(const char* left, const StdExt::String& right);
STD_EXT_EXPORT bool operator<(const std::string& left, const StdExt::String& right);
STD_EXT_EXPORT bool operator<(const std::string_view& left, const StdExt::String& right);

STD_EXT_EXPORT bool operator<=(const char* left, const StdExt::String& right);
STD_EXT_EXPORT bool operator<=(const std::string& left, const StdExt::String& right);
STD_EXT_EXPORT bool operator<=(const std::string_view& left, const StdExt::String& right);

STD_EXT_EXPORT bool operator==(const char* left, const StdExt::String& right);
STD_EXT_EXPORT bool operator==(const std::string& left, const StdExt::String& right);
STD_EXT_EXPORT bool operator==(const std::string_view& left, const StdExt::String& right);

STD_EXT_EXPORT bool operator>=(const char* left, const StdExt::String& right);
STD_EXT_EXPORT bool operator>=(const std::string& left, const StdExt::String& right);
STD_EXT_EXPORT bool operator>=(const std::string_view& left, const StdExt::String& right);

STD_EXT_EXPORT bool operator>(const char* left, const StdExt::String& right);
STD_EXT_EXPORT bool operator>(const std::string& left, const StdExt::String& right);
STD_EXT_EXPORT bool operator>(const std::string_view& left, const StdExt::String& right);

#endif // !_STD_EXT_STRING_H_