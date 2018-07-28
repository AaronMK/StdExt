#if defined(_MSC_VER) && !defined(_SCL_SECURE_NO_WARNINGS)
#	define _SCL_SECURE_NO_WARNINGS
#endif

#include <StdExt/String.h>

#include <cassert>

namespace StdExt
{
	String String::Literal(const char* str)
	{
		String ret;

		ret.mStrImp.emplace<StringLiteral>(str);
		ret.mView = std::get<StringLiteral>(ret.mStrImp).mLiteralView;

		return ret;
	}

	String String::join(const std::vector<String>& strings, std::string_view glue)
	{
		size_t length = 0;

		for  (String str : strings)
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
		mStrImp.emplace<StringLiteral>();
	}

	String::String(const char* str)
		: String(std::string_view(str))
	{
	}

	String::String(String&& other) noexcept
	{
		moveFrom(std::forward<String&&>(other));
	}

	String::String(const String& other) noexcept
	{
		copyFrom(other);
	}

	String::String(std::string_view str)
	{
		if (str.data() == nullptr)
		{
			mStrImp.emplace<StringLiteral>();
			mView = str;
		}
		else if (str.size() <= SmallString::MAX_SIZE)
		{
			mStrImp.emplace<SmallString>(str);
			mView = std::get<SmallString>(mStrImp).view();
		}
		else
		{
			mStrImp.emplace<LargeString>(str);
			mView = std::get<LargeString>(mStrImp).view();
		}
	}

	String::String(MemoryReference&& mem) noexcept
	{
		mStrImp.emplace<LargeString>(std::forward<MemoryReference&&>(mem));
		mView = std::get<LargeString>(mStrImp).view();
	}

	String::String(const MemoryReference& mem) noexcept
	{
		mStrImp.emplace<LargeString>(mem);
		mView = std::get<LargeString>(mStrImp).view();
	}

	String::String(const std::string& stdStr)
		: String(std::string_view(stdStr))
	{
	}

	String::String(std::string&& stdStr) noexcept
	{
		mStrImp.emplace<StdString>(std::move(stdStr));
		mView = std::get<StdString>(mStrImp).mStdString;
	}

	void String::moveFrom(String&& other)
	{
		if (std::holds_alternative<StringLiteral>(other.mStrImp))
		{
			mView = std::get<StringLiteral>(other.mStrImp).mLiteralView;
			mStrImp.emplace<StringLiteral>(mView);
		}
		else if (std::holds_alternative<SmallString>(other.mStrImp))
		{
			mStrImp.emplace<SmallString>(std::get<SmallString>(other.mStrImp));
			mView = std::get<SmallString>(mStrImp).view();
		}
		else if (std::holds_alternative<LargeString>(other.mStrImp))
		{
			mStrImp.emplace<LargeString>(std::move(std::get<LargeString>(other.mStrImp)));
			mView = std::get<LargeString>(mStrImp).view();
		}
		else if (std::holds_alternative<StdString>(other.mStrImp))
		{
			mStrImp.emplace<StdString>(std::move(std::get<StdString>(other.mStrImp)));
			mView = std::get<StdString>(mStrImp).mStdString;
		}

		other.mStrImp.emplace<StringLiteral>();
	}

	void String::copyFrom(const String& other)
	{
		if (std::holds_alternative<StringLiteral>(other.mStrImp))
		{
			mView = std::get<StringLiteral>(other.mStrImp).mLiteralView;
			mStrImp.emplace<StringLiteral>(mView);
		}
		else if (std::holds_alternative<SmallString>(other.mStrImp))
		{
			mStrImp.emplace<SmallString>(std::get<SmallString>(other.mStrImp));
			mView = std::get<SmallString>(mStrImp).view();
		}
		else if (std::holds_alternative<LargeString>(other.mStrImp))
		{
			mStrImp.emplace<LargeString>(std::get<LargeString>(other.mStrImp));
			mView = std::get<LargeString>(mStrImp).view();
		}
		else if (std::holds_alternative<StdString>(other.mStrImp))
		{
			std::string_view otherView = std::get<StdString>(other.mStrImp).mStdString;

			if (otherView.size() <= SmallString::MAX_SIZE)
			{
				mStrImp.emplace<SmallString>(otherView);
				mView = std::get<SmallString>(mStrImp).view();
			}
			else
			{
				mStrImp.emplace<LargeString>(otherView);
				mView = std::get<LargeString>(mStrImp).view();
			}
		}
	}

	std::string String::toStdString() const
	{
		return std::string(mView);
	}

	String& String::operator=(String&& other) noexcept
	{
		moveFrom(std::move(other));
		return *this;
	}

	String& String::operator=(const String& other)
	{
		copyFrom(other);
		return *this;
	}

	String& String::operator=(const std::string& stdStr)
	{
		if (stdStr.size() <= SmallString::MAX_SIZE)
		{
			mStrImp.emplace<SmallString>(stdStr);
			mView = std::get<SmallString>(mStrImp).view();
		}
		else
		{
			mStrImp.emplace<LargeString>(stdStr);
			mView = std::get<LargeString>(mStrImp).view();
		}

		return *this;
	}

	String& String::operator=(std::string&& stdStr)
	{
		mStrImp.emplace<StdString>(std::move(stdStr));
		mView = std::get<StdString>(mStrImp).mStdString;

		return *this;
	}

	bool String::operator==(const String& other) const
	{
		return (mView == other.mView);
	}

	bool String::operator!=(const String& other) const
	{
		return (mView != other.mView);
	}

	bool String::operator<(const String& other) const
	{
		return (mView < other.mView);
	}

	bool String::operator>(const String& other) const
	{
		return (mView > other.mView);
	}

	bool String::operator<=(const String& other) const
	{
		return (mView <= other.mView);
	}

	bool String::operator>=(const String& other) const
	{
		return (mView >= other.mView);
	}

	char String::operator[](size_t index) const
	{
		return mView[index];
	}

	int String::compare(std::string_view other) const
	{
		return mView.compare(other);
	}

	size_t String::length() const
	{
		return mView.length();
	}

	size_t String::size() const
	{
		return mView.size();
	}

	size_t String::copy(char* dest, size_t count, size_t pos) const
	{
		return mView.copy(dest, count, pos);
	}

	String String::substr(size_t pos, size_t count) const
	{
		if (std::holds_alternative<StringLiteral>(mStrImp))
		{
			std::string_view subView = mView.substr(pos, count);

			String ret;
			ret.mStrImp.emplace<StringLiteral>(subView);
			ret.mView = subView;

			return ret;
		}
		else if (std::holds_alternative<StdString>(mStrImp) || std::holds_alternative<SmallString>(mStrImp))
		{
			return String(mView.substr(pos, count));
		}
		else if (std::holds_alternative<LargeString>(mStrImp))
		{
			std::string_view subStr = mView.substr(pos, count);

			if (subStr.size() <= SmallString::MAX_SIZE)
			{
				return String(subStr);
			}
			else
			{
				String ret;
				ret.mStrImp.emplace<LargeString>(std::get<LargeString>(mStrImp).mMemory, subStr);
				ret.mView = subStr;

				return ret;
			}
		}

		throw not_implemented("No handling for type in string implementation.");
	}

	size_t String::find(std::string_view v, size_t pos) const
	{
		return mView.find(v, pos);
	}

	size_t String::find(char c, size_t pos) const
	{
		return mView.find(c, pos);
	}

	size_t String::find(const char* c, size_t pos, size_t count) const
	{
		return mView.find(c, pos, count);
	}

	size_t String::find(const char* c, size_t pos) const
	{
		return mView.find(c, pos);
	}

	size_t String::rfind(std::string_view v, size_t pos) const
	{
		return mView.rfind(v, pos);
	}

	size_t String::rfind(char c, size_t pos) const
	{
		return mView.rfind(c, pos);
	}

	size_t String::rfind(const char* c, size_t pos, size_t count) const
	{
		return mView.rfind(c, pos, count);
	}

	size_t String::rfind(const char* c, size_t pos) const
	{
		return mView.rfind(c, pos);
	}

	size_t String::find_first_of(std::string_view v, size_t pos) const
	{
		return mView.find_first_of(v, pos);
	}

	size_t String::find_first_of(char c, size_t pos) const
	{
		return mView.find_first_of(c, pos);
	}

	size_t String::find_first_of(const char* c, size_t pos, size_t count) const
	{
		return mView.find_first_of(c, pos, count);
	}

	size_t String::find_first_of(const char* c, size_t pos) const
	{
		return mView.find_first_of(c, pos);
	}

	size_t String::find_last_of(std::string_view v, size_t pos) const
	{
		return mView.find_last_of(v, pos);
	}

	size_t String::find_last_of(char c, size_t pos) const
	{
		return mView.find_last_of(c, pos);
	}

	size_t String::find_last_of(const char* c, size_t pos, size_t count) const
	{
		return mView.find_last_of(c, pos, count);
	}

	size_t String::find_last_of(const char* c, size_t pos) const
	{
		return mView.find_last_of(c, pos);
	}

	size_t String::find_first_not_of(std::string_view v, size_t pos) const
	{
		return mView.find_first_not_of(v, pos);
	}

	size_t String::find_first_not_of(char c, size_t pos) const
	{
		return mView.find_first_not_of(c, pos);
	}

	size_t String::find_first_not_of(const char* c, size_t pos, size_t count) const
	{
		return mView.find_first_not_of(c, pos, count);
	}

	size_t String::find_first_not_of(const char* c, size_t pos) const
	{
		return mView.find_first_not_of(c, pos);
	}

	size_t String::find_last_not_of(std::string_view v, size_t pos) const
	{
		return mView.find_last_not_of(v, pos);
	}

	size_t String::find_last_not_of(char c, size_t pos) const
	{
		return mView.find_last_not_of(c, pos);
	}

	size_t String::find_last_not_of(const char* c, size_t pos, size_t count) const
	{
		return mView.find_last_not_of(c, pos, count);
	}

	size_t String::find_last_not_of(const char* c, size_t pos) const
	{
		return mView.find_last_not_of(c, pos);
	}

	std::vector<String> String::split(std::string_view deliminator, bool keepEmpty) const
	{
		std::vector<String> ret;

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
			ret.push_back(substr(begin, strSize - begin));
		else if (begin == strSize && keepEmpty)
			ret.emplace_back("");

		return ret;
	}

	String::operator std::string_view() const
	{
		return mView;
	}

	//////////////////////////
	
	String::SmallString::SmallString()
	{
		mSize = 0;
		mBuffer[0] = 0;
	}

	String::SmallString::SmallString(std::string_view str)
	{
		assert(str.size() <= MAX_SIZE && str.data() != nullptr);
		
		memcpy_s(&mBuffer[0], sizeof(mBuffer), str.data(), str.size());
		mSize = str.size();
	}

	String::SmallString::SmallString(const SmallString& other) noexcept
	{
		copyFrom(other);
	}

	String::SmallString::SmallString(SmallString&& other) noexcept
	{
		copyFrom(other);
	}

	String::SmallString& StdExt::String::SmallString::operator=(SmallString&& other) noexcept
	{
		copyFrom(other);
		return *this;
	}

	String::SmallString& StdExt::String::SmallString::operator=(const SmallString& other) noexcept
	{
		copyFrom(other);
		return *this;
	}

	std::string_view String::SmallString::view() const
	{
		return std::string_view(&mBuffer[0], mSize);
	}

	String::SmallString String::SmallString::substr(size_t pos, size_t count) const
	{
		return SmallString(view().substr(pos, count));
	}

	void StdExt::String::SmallString::copyFrom(const SmallString& other)
	{
		memcpy_s(&mBuffer[0], sizeof(mBuffer), &other.mBuffer[0], other.mSize);
		mSize = other.mSize;
	}

	//////////////////////////

	String::LargeString::LargeString(std::string_view str)
		: mMemory(str.size())
	{
		assert(str.size() > SmallString::MAX_SIZE && str.data() != nullptr);

		memcpy_s(mMemory.data(), mMemory.size(), str.data(), str.size());
		mLargeView = std::string_view((const char*)mMemory.data(), mMemory.size());
	}

	StdExt::String::LargeString::LargeString(MemoryReference&& memRef)
		: mMemory(std::move(memRef))
	{
		mLargeView = std::string_view((const char*)mMemory.data(), mMemory.size());
	}

	StdExt::String::LargeString::LargeString(const MemoryReference& memRef)
		: mMemory(memRef)
	{
		mLargeView = std::string_view((const char*)mMemory.data(), mMemory.size());
	}

	StdExt::String::LargeString::LargeString(MemoryReference&& memRef, std::string_view strView)
		: mLargeView(strView), mMemory(std::move(memRef))
	{
	}

	StdExt::String::LargeString::LargeString(const MemoryReference& memRef, std::string_view strView)
		: mLargeView(strView), mMemory(memRef)
	{
	}

	std::string_view String::LargeString::view() const
	{
		return std::string_view((const char*)mMemory.data(), mMemory.size());
	}

	//////////////////////////

	String::StringLiteral::StringLiteral()
	{
	}

	String::StringLiteral::StringLiteral(std::string_view str)
		: mLiteralView(str)
	{
	}

	String String::StringLiteral::substr(size_t pos, size_t count) const
	{
		return String(mLiteralView.substr(pos, count));
	}

	//////////////////////////

	String::StdString::StdString(std::string&& str)
	{
		mStdString = std::move(str);
	}

	String::StdString::~StdString()
	{
	}
}