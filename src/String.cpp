#include <StdExt/String.h>

#include <cassert>

namespace StdExt
{
	String String::Literal(const char* str)
	{
		String ret;
		ret.mStrImp.setValue<StringLiteral>(str);

		return ret;
	}

	String String::join(const std::vector<String>& strings, std::string_view glue)
	{
		size_t length = 0;

		for each (String str in strings)
			length += str.length();

		length += (strings.size() - 1) * glue.length();

		MemoryReference memory(length);

		char* start = (char*)memory.data();
		size_t index = 0;

		for (int i = 0; i < strings.size(); ++i)
		{
			strings[i].copy(&start[index], strings[i].length());
			index += strings[i].length();

			if (i != strings.size() - 1)
			{
				glue.copy(&start[index], glue.length());
				index += glue.length();
			}
		}

		return String(std::move(memory));
	}

	String::String() noexcept
	{
		mStrImp.setValue<StringLiteral>();
	}

	String::String(const char* str)
		: String(std::string_view(str))
	{
	}

	String::String(const String& other) noexcept
	{
		if (other.mStrImp.typeInfo() == typeid(StdString))
		{
			if (other.length() <= SmallString::MAX_SIZE)
				mStrImp.setValue<SmallString>(other);
			else
				mStrImp.setValue<LargeString>(other);
		}
		else
		{
			mStrImp = other.mStrImp;
		}
	}

	String::String(std::string_view str)
	{
		if (str.data() == nullptr)
			mStrImp.setValue<StringLiteral>();
		else if (str.size() <= SmallString::MAX_SIZE)
			mStrImp.setValue<SmallString>(str);
		else
			mStrImp.setValue<LargeString>(str);
	}

	String::String(MemoryReference&& mem) noexcept
	{
		mStrImp.setValue<LargeString>(std::move(mem));
	}

	String::String(const MemoryReference& mem) noexcept
	{
		mStrImp.setValue<LargeString>(mem);
	}

	String::String(const std::string& stdStr)
		: String(std::string_view(stdStr))
	{
	}

	String::String(std::string&& stdStr) noexcept
	{
		mStrImp.setValue<StdString>(std::move(stdStr));
	}

	String::String(String::InPlace_t&& inPlace)
		: mStrImp(std::move(inPlace))
	{
	}

	std::string String::toStdString() const
	{
		std::string_view view = mStrImp->View;
		return std::string(view.data(), view.size());
	}

	String& String::operator=(String&& other) noexcept
	{
		mStrImp = std::move(other.mStrImp);
		return *this;
	}

	String& String::operator=(const String& other)
	{
		if (other.mStrImp.typeInfo() == typeid(StdString))
		{
			if (other.length() <= SmallString::MAX_SIZE)
				mStrImp.setValue<SmallString>(other);
			else
				mStrImp.setValue<LargeString>(other);
		}
		else
		{
			mStrImp = other.mStrImp;
		}

		return *this;
	}

	String& String::operator=(const std::string& stdStr)
	{
		if (stdStr.size() <= SmallString::MAX_SIZE)
			mStrImp.setValue<SmallString>(stdStr);
		else
			mStrImp.setValue<LargeString>(stdStr);

		return *this;
	}

	String& String::operator=(std::string&& stdStr)
	{
		mStrImp.setValue<StdString>(std::move(stdStr));
		return *this;
	}

	bool String::operator==(const String& other) const
	{
		return (mStrImp->View == other.mStrImp->View);
	}

	bool String::operator!=(const String& other) const
	{
		return (mStrImp->View != other.mStrImp->View);
	}

	bool String::operator<(const String& other) const
	{
		return (mStrImp->View < other.mStrImp->View);
	}

	bool String::operator>(const String& other) const
	{
		return (mStrImp->View > other.mStrImp->View);
	}

	bool String::operator<=(const String& other) const
	{
		return (mStrImp->View <= other.mStrImp->View);
	}

	bool String::operator>=(const String& other) const
	{
		return (mStrImp->View >= other.mStrImp->View);
	}

	char String::operator[](size_t index) const
	{
		return mStrImp->View[index];
	}

	int String::compare(std::string_view other) const
	{
		return mStrImp->View.compare(other);
	}

	size_t String::length() const
	{
		return mStrImp->View.length();
	}

	size_t String::size() const
	{
		return mStrImp->View.size();
	}

	size_t String::copy(char* dest, size_t count, size_t pos) const
	{
		return mStrImp->View.copy(dest, count, pos);
	}

	String String::substr(size_t pos, size_t count) const
	{
		return mStrImp->substr(pos, count);
	}

	size_t String::find(std::string_view v, size_t pos) const
	{
		return mStrImp->View.find(v, pos);
	}

	size_t String::find(char c, size_t pos) const
	{
		return mStrImp->View.find(c, pos);
	}

	size_t String::find(const char* c, size_t pos, size_t count) const
	{
		return mStrImp->View.find(c, pos, count);
	}

	size_t String::find(const char* c, size_t pos) const
	{
		return mStrImp->View.find(c, pos);
	}

	size_t String::rfind(std::string_view v, size_t pos) const
	{
		return mStrImp->View.rfind(v, pos);
	}

	size_t String::rfind(char c, size_t pos) const
	{
		return mStrImp->View.rfind(c, pos);
	}

	size_t String::rfind(const char* c, size_t pos, size_t count) const
	{
		return mStrImp->View.rfind(c, pos, count);
	}

	size_t String::rfind(const char* c, size_t pos) const
	{
		return mStrImp->View.rfind(c, pos);
	}

	size_t String::find_first_of(std::string_view v, size_t pos) const
	{
		return mStrImp->View.find_first_of(v, pos);
	}

	size_t String::find_first_of(char c, size_t pos) const
	{
		return mStrImp->View.find_first_of(c, pos);
	}

	size_t String::find_first_of(const char* c, size_t pos, size_t count) const
	{
		return mStrImp->View.find_first_of(c, pos, count);
	}

	size_t String::find_first_of(const char* c, size_t pos) const
	{
		return mStrImp->View.find_first_of(c, pos);
	}

	size_t String::find_last_of(std::string_view v, size_t pos) const
	{
		return mStrImp->View.find_last_of(v, pos);
	}

	size_t String::find_last_of(char c, size_t pos) const
	{
		return mStrImp->View.find_last_of(c, pos);
	}

	size_t String::find_last_of(const char* c, size_t pos, size_t count) const
	{
		return mStrImp->View.find_last_of(c, pos, count);
	}

	size_t String::find_last_of(const char* c, size_t pos) const
	{
		return mStrImp->View.find_last_of(c, pos);
	}

	size_t String::find_first_not_of(std::string_view v, size_t pos) const
	{
		return mStrImp->View.find_first_not_of(v, pos);
	}

	size_t String::find_first_not_of(char c, size_t pos) const
	{
		return mStrImp->View.find_first_not_of(c, pos);
	}

	size_t String::find_first_not_of(const char* c, size_t pos, size_t count) const
	{
		return mStrImp->View.find_first_not_of(c, pos, count);
	}

	size_t String::find_first_not_of(const char* c, size_t pos) const
	{
		return mStrImp->View.find_first_not_of(c, pos);
	}

	size_t String::find_last_not_of(std::string_view v, size_t pos) const
	{
		return mStrImp->View.find_last_not_of(v, pos);
	}

	size_t String::find_last_not_of(char c, size_t pos) const
	{
		return mStrImp->View.find_last_not_of(c, pos);
	}

	size_t String::find_last_not_of(const char* c, size_t pos, size_t count) const
	{
		return mStrImp->View.find_last_not_of(c, pos, count);
	}

	size_t String::find_last_not_of(const char* c, size_t pos) const
	{
		return mStrImp->View.find_last_not_of(c, pos);
	}

	std::vector<String> String::split(std::string_view deliminator, bool keepEmpty) const
	{
		std::vector<String> ret;

		size_t strSize = size();
		size_t begin = 0;
		size_t end = 0;

		while (begin < strSize && end != npos)
		{
			end = find(deliminator, begin);

			if (end != npos && (keepEmpty || end != begin))
			{
				ret.push_back(substr(begin, end - begin));
				begin = end + deliminator.size();
			}
		}

		return ret;
	}

	String::operator std::string_view() const
	{
		return mStrImp->View;
	}

	//////////////////////////

	String::StrImp::StrImp()
	{
	}

	String::StrImp::~StrImp()
	{
	}

	//////////////////////////
	
	String::SmallString::SmallString(std::string_view str)
	{
		assert(str.size() <= MAX_SIZE && str.data() != nullptr);
		
		memcpy_s(&mBuffer[0], sizeof(mBuffer), str.data(), str.size());
		View = std::string_view(&mBuffer[0], str.size());
	}

	String::SmallString::SmallString(const SmallString& other)
	{
		memcpy_s(&mBuffer[0], sizeof(mBuffer), other.View.data(), other.View.size());
		View = std::string_view(&mBuffer[0], other.View.size());
	}

	String::SmallString::~SmallString()
	{
	}

	String String::SmallString::substr(size_t pos, size_t count) const
	{
		return String(View.substr(pos, count));
	}

	//////////////////////////

	String::LargeString::LargeString(std::string_view str)
		: mMemory(str.size())
	{
		assert(str.size() > SmallString::MAX_SIZE && str.data() != nullptr);

		memcpy_s(mMemory.data(), mMemory.size(), str.data(), str.size());
		View = std::string_view((char*)mMemory.data(), mMemory.size());
	}

	String::LargeString::LargeString(const MemoryReference& mem)
		: mMemory(mem)
	{
		View = std::string_view((char*)mMemory.data(), mMemory.size());
	}

	String::LargeString::LargeString(MemoryReference&& mem)
		: mMemory(std::move(mem))
	{
		View = std::string_view((char*)mMemory.data(), mMemory.size());
	}

	String::LargeString::~LargeString()
	{
	}

	String String::LargeString::substr(size_t pos, size_t count) const
	{
		std::string_view subView = View.substr(pos, count);
		
		if (subView.length() <= SmallString::MAX_SIZE)
		{
			return String(InPlace_t::make<SmallString>(subView));
		}
		else
		{
			InPlace_t lrgString = InPlace_t::make<LargeString>(mMemory);
			lrgString->View = subView;

			return String(std::move(lrgString));
		}
	}

	//////////////////////////

	String::StringLiteral::StringLiteral()
	{
	}

	String::StringLiteral::StringLiteral(std::string_view str)
	{
		View = str;
	}

	String::StringLiteral::~StringLiteral()
	{
	}

	String String::StringLiteral::substr(size_t pos, size_t count) const
	{
		return String(View.substr(pos, count));
	}

	//////////////////////////

	String::StdString::StdString(std::string&& str)
	{
		mStdString = std::move(str);
		View = mStdString;
	}

	String::StdString::~StdString()
	{
	}

	String String::StdString::substr(size_t pos, size_t count) const
	{
		return String(View.substr(pos, count));
	}
}