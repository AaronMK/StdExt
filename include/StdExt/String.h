#ifndef _STD_EXT_STRING_H_
#define _STD_EXT_STRING_H_

#include "Memory.h"
#include "InPlace.h"

#include <vector>
#include <variant>

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

namespace StdExt
{
	class STD_EXT_EXPORT String
	{
	public:
		static constexpr size_t npos = std::string_view::npos;

		static String Literal(const char* str);

		static String join(const std::vector<String>& strings, std::string_view glue);

		String() noexcept;
		
		String(const char* str);
		String(const char* str, size_t size);
		String(String&& other) noexcept;
		String(const String& other) noexcept;
		String(std::string_view str);

		String(MemoryReference&& mem) noexcept;
		String(const MemoryReference& mem) noexcept;

		explicit String(const std::string& stdStr);
		explicit String(std::string&& stdStr) noexcept;

		std::string toStdString() const;

		String& operator=(String&& other) noexcept;
		String& operator=(const String& other);

		String& operator=(const std::string& stdStr);
		String& operator=(std::string&& stdStr);

		bool operator==(const String& other) const;
		bool operator!=(const String& other) const; 
		bool operator<(const String& other) const;
		bool operator>(const String& other) const;
		bool operator<=(const String& other) const;
		bool operator>=(const String& other) const;

		char operator[](size_t index) const;

		int compare(std::string_view other) const;

		size_t length() const;
		size_t size() const;

		size_t copy(char* dest, size_t count, size_t pos = 0) const;

		String substr(size_t pos, size_t count = npos) const;

		size_t find(std::string_view v, size_t pos = 0) const;
		size_t find(char c, size_t pos = 0) const;
		size_t find(const char* c, size_t pos, size_t count) const;
		size_t find(const char* c, size_t pos = 0) const;

		size_t rfind(std::string_view v, size_t pos = 0) const;
		size_t rfind(char c, size_t pos = 0) const;
		size_t rfind(const char* c, size_t pos, size_t count) const;
		size_t rfind(const char* c, size_t pos = 0) const;

		size_t find_first_of(std::string_view v, size_t pos = 0) const;
		size_t find_first_of(char c, size_t pos = 0) const;
		size_t find_first_of(const char* c, size_t pos, size_t count) const;
		size_t find_first_of(const char* c, size_t pos = 0) const;

		size_t find_last_of(std::string_view v, size_t pos = 0) const;
		size_t find_last_of(char c, size_t pos = 0) const;
		size_t find_last_of(const char* c, size_t pos, size_t count) const;
		size_t find_last_of(const char* c, size_t pos = 0) const;

		size_t find_first_not_of(std::string_view v, size_t pos = 0) const;
		size_t find_first_not_of(char c, size_t pos = 0) const;
		size_t find_first_not_of(const char* c, size_t pos, size_t count) const;
		size_t find_first_not_of(const char* c, size_t pos = 0) const;

		size_t find_last_not_of(std::string_view v, size_t pos = 0) const;
		size_t find_last_not_of(char c, size_t pos = 0) const;
		size_t find_last_not_of(const char* c, size_t pos, size_t count) const;
		size_t find_last_not_of(const char* c, size_t pos = 0) const;

		std::vector<String> split(std::string_view deliminator, bool keepEmpty = true) const;

		/**
		 * @brief
		 *  Returns true if the internal storage of the string is null-terminated.  If true, the
		 *  character pointer returned by data() can be passed directly into c-style functions.
		 */
		bool isNullTerminated() const;

		/**
		 * @brief
		 *  Returns a &String for which data() will return a null-terminated c-style string and
		 *  isNullTerminated() will be true.
		 */
		String getNullTerminated() const;

		const char* data() const;

		operator std::string_view() const;

	private:
		class StringLiteral
		{
		public:
			StringLiteral(const StringLiteral& str) = default;
			StringLiteral(StringLiteral&& str) = default;

			StringLiteral();
			StringLiteral(std::string_view str);

			StringLiteral& operator=(StringLiteral&& other) = default;
			StringLiteral& operator=(const StringLiteral& other) = default;

			std::string_view mLiteralView;

			String substr(size_t pos, size_t count) const;
		};
		
		class SmallString
		{
		public:
			static constexpr size_t MAX_SIZE = sizeof(std::string) - sizeof(size_t) - 1;

			SmallString();
			SmallString(std::string_view str);
			SmallString(void* data, size_t size);

			SmallString(const SmallString& other) noexcept;
			SmallString(SmallString&& other) noexcept;

			SmallString& operator=(SmallString&& other) noexcept;
			SmallString& operator=(const SmallString& other) noexcept;

			std::string_view view() const;
			SmallString substr(size_t pos, size_t count) const;

			char mBuffer[MAX_SIZE + 1];
			size_t mSize;

		private:
			void copyFrom(const SmallString& other);
		};

		class LargeString
		{
		public:
			LargeString(const LargeString& str) = default;
			LargeString(LargeString&& str) = default;

			LargeString(std::string_view str);

			LargeString(MemoryReference&& memRef);
			LargeString(const MemoryReference& memRef);

			LargeString(MemoryReference&& memRef, std::string_view strView);
			LargeString(const MemoryReference& memRef, std::string_view strView);

			LargeString& operator=(LargeString&& other) = default;
			LargeString& operator=(const LargeString& other) = default;

			std::string_view view() const;

			std::string_view mLargeView;
			MemoryReference mMemory;
		};

		class StdString
		{
		public:
			StdString(StdString&& str) = default;
			StdString(const StdString& str) = delete;

			StdString(std::string&& str);
			~StdString();

			StdString& operator=(StdString&& other) = default;
			StdString& operator=(const StdString& other) = delete;

			std::string mStdString;
		};

		using Imp_Varient = std::variant<StringLiteral, SmallString, LargeString, StdString>;

		void moveFrom(String&& other);
		void copyFrom(const String& other);

		Imp_Varient mStrImp;
		std::string_view mView;

	public:

		/**
		 * @brief
		 *  The maximum string length for which a seperate memory allocation will not occur.
		 *  Strings at or below this length are stored directly in the string object.
		 */
		static constexpr size_t SmallSize = SmallString::MAX_SIZE;
	};
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // !_STD_EXT_STRING_H_