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

		String(String&& other) = default;

		String() noexcept;
		
		String(const char* str);
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

		operator std::string_view() const;

	private:
		friend class StrImp;

		class StrImp
		{
		public:
			StrImp();
			virtual ~StrImp();

			std::string_view View;

			virtual String substr(size_t pos, size_t count) const = 0;
		};

		class SmallString : public StrImp
		{
		public:
			static constexpr size_t MAX_SIZE = sizeof(std::string);

			SmallString(std::string_view str);
			SmallString(const SmallString& other);

			virtual ~SmallString();

			virtual String substr(size_t pos, size_t count) const override;

		private:
			char mBuffer[MAX_SIZE];
		};

		class LargeString : public StrImp
		{
		public:
			LargeString(const LargeString& str) = default;
			LargeString(LargeString&& str) = default;

			LargeString(const MemoryReference& mem);
			LargeString(MemoryReference&& mem);
			LargeString(std::string_view str);
			virtual ~LargeString();

			LargeString& operator=(LargeString&& other) = default;
			LargeString& operator=(const LargeString& other) = default;

			virtual String substr(size_t pos, size_t count) const override;

		private:
			MemoryReference mMemory;
		};

		class StringLiteral : public StrImp
		{
		public:
			StringLiteral();
			StringLiteral(std::string_view str);

			virtual ~StringLiteral();

			virtual String substr(size_t pos, size_t count) const override;
		};

		class StdString : public StrImp
		{
		public:
			StdString(std::string&& str);
			virtual ~StdString();

			virtual String substr(size_t pos, size_t count) const override;

		private:
			std::string mStdString;
		};

		static constexpr size_t INPLACE_SIZE =
			AlignedBlockSize_v<SmallString, LargeString, StringLiteral, StdString>;

		using InPlace_t = InPlace<StrImp, INPLACE_SIZE, true>;

		String(InPlace_t&& inPlace);

		InPlace_t mStrImp;
	};
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // !_STD_EXT_STRING_H_