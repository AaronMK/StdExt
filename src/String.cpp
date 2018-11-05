#if defined(_MSC_VER) && !defined(_SCL_SECURE_NO_WARNINGS)
#	define _SCL_SECURE_NO_WARNINGS
#endif

#include <StdExt/String.h>

#include <cassert>
#include "..\include\StdExt\String.h"

namespace StdExt
{
	String String::Literal(const char* str)
	{
		String ret;

		ret.mView = std::string_view(str);
		ret.mIsLiteral = true;

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
		mIsLiteral = false;
	}

	String::String(const char* str)
		: String(std::string_view(str))
	{
	}

	String::String(const char* str, size_t size)
		: String(std::string_view(str, size))
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

	String::String(const std::string_view& str)
	{
		copyFrom(str);
	}

	String::String(MemoryReference&& mem) noexcept
	{
		if (mem.size() <= SmallSize)
		{
			copyFrom(std::string_view((const char*)mem.data(), mem.size()));
			mem.makeNull();
		}
		else
		{
			mIsLiteral = false;
			mHeapMemory = std::move(mem);
			mView = std::string_view((const char*)mHeapMemory.data(), mHeapMemory.size());
		}
	}

	String::String(const MemoryReference& mem) noexcept
	{
		if (mem.size() <= SmallSize)
		{
			copyFrom(std::string_view((const char*)mem.data(), mem.size()));
		}
		else
		{
			mIsLiteral = false;
			mHeapMemory = mem;
			mView = std::string_view((const char*)mHeapMemory.data(), mHeapMemory.size());
		}
	}

	String::String(const std::string& stdStr)
		: String(std::string_view(stdStr))
	{
	}

	String::String(std::string&& stdStr)
		: String(std::string_view(stdStr))
	{
		stdStr.clear();
	}

	void String::moveFrom(String&& other)
	{
		mView = other.mView;
		mIsLiteral = other.mIsLiteral;
		mHeapMemory = std::move(other.mHeapMemory);
		
		other.mView = std::string_view();
		other.mIsLiteral = false;

		if (mIsLiteral || mView.data() == nullptr || mHeapMemory )
			return;

		memcpy(mSmallMemory, mView.data(), mView.size());
		mSmallMemory[mView.size()] = '\0';

		mView = std::string_view(mSmallMemory, mView.size());
	}

	void String::copyFrom(const String& other)
	{
		mIsLiteral = other.mIsLiteral;
		mView = other.mView;

		if (mIsLiteral || mView.data() == nullptr)
		{
			mHeapMemory.makeNull();
		}
		else if (mView.size() <= SmallSize)
		{
			memcpy(mSmallMemory, mView.data(), mView.size());
			mSmallMemory[mView.size()] = '\0';

			mView = std::string_view(mSmallMemory, mView.size());
		}
		else
		{
			mHeapMemory = other.mHeapMemory;
		}
	}

	void String::copyFrom(const std::string_view& view)
	{
		mIsLiteral = false;

		if (view.size() <= SmallSize)
		{
			mHeapMemory.makeNull();

			if (view.data() == nullptr)
			{
				mView = std::string_view();
			}
			else
			{
				memcpy(mSmallMemory, view.data(), view.size());
				mSmallMemory[view.size()] = '\0';

				mView = std::string_view(mSmallMemory, view.size());
			}
		}
		else
		{
			mHeapMemory = MemoryReference(view.size() + 1);
			char* heapBegin = (char*)mHeapMemory.data();

			memcpy(heapBegin, view.data(), view.size());
			heapBegin[view.size()] = '0';
			
			mView = std::string_view(heapBegin, view.size());
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
		copyFrom(std::string_view(stdStr));
		return *this;
	}

	String& String::operator=(std::string&& stdStr)
	{
		copyFrom(std::string_view(stdStr));
		stdStr.clear();

		return *this;
	}

	String& StdExt::String::operator=(const char* str)
	{
		copyFrom(std::string_view(str));
		return *this;
	}

	bool StdExt::String::operator==(const char* other) const
	{
		return (mView.compare(other) == 0);
	}

	bool String::operator==(const String& other) const
	{
		return (mView == other.mView);
	}

	bool String::operator==(const std::string_view& other) const
	{
		return (mView == other);
	}

	bool String::operator!=(const char* other) const
	{
		return (mView.compare(other) != 0);
	}

	bool String::operator!=(const String& other) const
	{
		return (mView != other.mView);
	}

	bool String::operator!=(const std::string_view& other) const
	{
		return (mView != other);
	}

	bool String::operator<(const char* other) const
	{
		return (mView.compare(other) < 0);
	}

	bool String::operator<(const String& other) const
	{
		return (mView < other.mView);
	}

	bool String::operator<(const std::string_view& other) const
	{
		return (mView < other);
	}

	bool String::operator>(const char* other) const
	{
		return (mView.compare(other) > 0);
	}

	bool String::operator>(const String& other) const
	{
		return (mView > other.mView);
	}

	bool String::operator>(const std::string_view& other) const
	{
		return (mView > other);
	}

	bool String::operator<=(const char* other) const
	{
		return (mView.compare(other) <= 0);
	}

	bool String::operator<=(const String& other) const
	{
		return (mView <= other.mView);
	}

	bool String::operator<=(const std::string_view& other) const
	{
		return (mView <= other);
	}

	bool String::operator>=(const char* other) const
	{
		return (mView.compare(other) >= 0);
	}

	bool String::operator>=(const String& other) const
	{
		return (mView >= other.mView);
	}

	bool String::operator>=(const std::string_view& other) const
	{
		return (mView >= other);
	}

	char String::operator[](size_t index) const
	{
		return mView[index];
	}

	int String::compare(std::string_view other) const
	{
		return mView.compare(other);
	}

	int String::compare(const char* other) const
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
		std::string_view subView = mView.substr(pos, count);

		if (mIsLiteral)
		{
			String ret;
			ret.mIsLiteral = true;
			ret.mView = subView;

			return ret;
		}
		else if (subView.size() <= SmallSize)
		{
			return String(subView);
		}
		else
		{
			String ret;
			ret.mView = subView;
			ret.mIsLiteral = false;
			ret.mHeapMemory = mHeapMemory;

			return ret;
		}
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
			ret.emplace_back(substr(begin, strSize - begin));
		else if (begin == strSize && keepEmpty)
			ret.emplace_back("");

		return ret;
	}

	bool String::isNullTerminated() const
	{
		if (mView.data() == nullptr)
			return false;

		const char* addrNullCheck = mView.data() + mView.size();

		if (mIsLiteral || !mHeapMemory)
			return *addrNullCheck == '\0';

		const char* addrLastMemory = (const char*)mHeapMemory.data() + mHeapMemory.size();

		if (addrLastMemory <= addrNullCheck)
			return *addrNullCheck == '\0';

		return false;
	}

	String String::getNullTerminated() const
	{
		if (isNullTerminated())
			return *this;
		else if (mView.data() == nullptr)
			return String::Literal("");
		else
			return String(mView);
	}

	const char* String::data() const
	{
		return mView.data();
	}

	String::operator std::string_view() const
	{
		return mView;
	}
}