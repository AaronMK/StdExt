#if defined(_MSC_VER) && !defined(_SCL_SECURE_NO_WARNINGS)
#	define _SCL_SECURE_NO_WARNINGS
#endif

#include <StdExt/String.h>

#include <cassert>
#include "..\include\StdExt\String.h"

namespace StdExt
{
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

	constexpr String::String() noexcept
		: mIsLiteral(false)
	{
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

		memcpy(mSmallMemory.data(), mView.data(), mView.size());
		mSmallMemory[mView.size()] = '\0';

		mView = std::string_view(mSmallMemory.data(), mView.size());
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
			memcpy(mSmallMemory.data(), mView.data(), mView.size());
			mSmallMemory[mView.size()] = '\0';

			mView = std::string_view(mSmallMemory.data(), mView.size());
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
				memcpy(mSmallMemory.data(), view.data(), view.size());
				mSmallMemory[view.size()] = '\0';

				mView = std::string_view(mSmallMemory.data(), view.size());
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

	String& String::operator=(const StringLiteral& other) noexcept
	{
		mView = other.mView;
		mHeapMemory.makeNull();

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

	bool String::operator==(const StringLiteral& other) const
	{
		return (mView.compare(other.mView) == 0);
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

	bool String::operator!=(const StringLiteral& other) const
	{
		return (mView.compare(other.mView) != 0);
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

	bool String::operator<(const StringLiteral& other) const
	{
		return (mView.compare(other.mView)< 0);
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

	bool String::operator>(const StringLiteral& other) const
	{
		return (mView.compare(other.mView) > 0);
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

	bool String::operator<=(const StringLiteral& other) const
	{
		return (mView.compare(other.mView) <= 0);
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

	bool String::operator>=(const StringLiteral& other) const
	{
		return (mView.compare(other.mView) >= 0);
	}

	bool String::operator>=(const std::string_view& other) const
	{
		return (mView >= other);
	}

	char String::operator[](size_t index) const
	{
		return mView[index];
	}

	String String::operator+(const char* other) const
	{
		return *this + std::string_view(other);
	}

	String String::operator+(const String& other) const
	{
		return *this + other.mView;
	}

	String String::operator+(const std::string& other) const
	{
		return *this + std::string_view(other);
	}

	String String::operator+(const StringLiteral& other) const
	{
		return *this + other.mView;
	}

	String String::operator+(const std::string_view& other) const
	{
		size_t combinedSize = size() + other.size();
		char* outMemory = nullptr;
		String ret;

		if (combinedSize <= SmallSize)
		{
			outMemory = ret.mSmallMemory.data();
		}
		else
		{
			ret.mHeapMemory = MemoryReference(combinedSize + 1);
			outMemory = (char*)ret.mHeapMemory.data();
		}

		memcpy(outMemory, data(), size());
		memcpy(outMemory + size(), other.data(), other.size());
		outMemory[combinedSize] = '\0';
		
		ret.mIsLiteral = false;
		ret.mView = std::string_view(outMemory, combinedSize);

		return ret;
	}


	String& String::operator+=(const char* other)
	{
		return *this += std::string_view(other);
	}

	String& String::operator+=(const String& other)
	{
		return *this += other.mView;
	}

	String& String::operator+=(const std::string& other)
	{
		return *this += std::string_view(other);
	}

	String& String::operator+=(const StringLiteral& other)
	{
		return *this += other.mView;
	}

	String& String::operator+=(const std::string_view& other)
	{
		size_t combinedSize = size() + other.size();

		if (combinedSize <= SmallSize)
		{
			if (mIsLiteral)
				memcpy(&mSmallMemory[0], data(), size());

			memcpy(&mSmallMemory[size()], other.data(), other.size());
			mView = std::string_view(&mSmallMemory[0], combinedSize);
		}
		else
		{
			MemoryReference memory(combinedSize + 1);

			memcpy(memory.data(), data(), size());
			memcpy((char*)memory.data() + size(), other.data(), other.size());

			mHeapMemory = memory;
			mView = std::string_view((char*)mHeapMemory.data(), combinedSize);
		}

		mIsLiteral = false;

		return *this;
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

	std::vector<String> String::split(char deliminator, bool keepEmpty) const
	{
		return split(std::string_view(&deliminator, 1), keepEmpty);
	}

	std::vector<String> String::split(const char* c, size_t pos, size_t count, bool keepEmpty) const
	{
		return split(std::string_view(&c[pos], count), keepEmpty);
	}

	std::vector<String> String::split(const char* c, size_t pos, bool keepEmpty) const
	{
		return split(std::string_view(&c[pos]), keepEmpty);
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
			return StringLiteral("");
		else
			return String(mView);
	}

	const char* String::data() const
	{
		return mView.data();
	}

	std::string_view String::view() const
	{
		return mView;
	}

	String::operator std::string_view() const
	{
		return mView;
	}

	class StringHelper
	{
	public:
		static String add(const std::string_view& left, const StdExt::String& right)
		{
			size_t combinedSize = left.size() + right.size();
			char* outMemory = nullptr;
			StdExt::String ret;

			if (combinedSize <= StdExt::String::SmallSize)
			{
				outMemory = ret.mSmallMemory.data();
			}
			else
			{
				ret.mHeapMemory = MemoryReference(combinedSize + 1);
				outMemory = (char*)ret.mHeapMemory.data();
			}

			memcpy(outMemory, left.data(), left.size());
			memcpy(outMemory + left.size(), right.data(), right.size());
			outMemory[combinedSize] = '\0';

			ret.mIsLiteral = false;
			ret.mView = std::string_view(outMemory, combinedSize);

			return ret;
		}
	};
}

std::ostream& operator<<(std::ostream& stream, const StdExt::String& str)
{
	return stream << str.getNullTerminated().data();
}

/////////////////////////////////////

StdExt::String operator+(const char* left, const StdExt::String& right)
{
	return StdExt::StringHelper::add(std::string_view(left), right);
}

StdExt::String operator+(const std::string& left, const StdExt::String& right)
{
	return StdExt::StringHelper::add(std::string_view(left), right);
}

StdExt::String operator+(const std::string_view& left, const StdExt::String& right)
{
	return StdExt::StringHelper::add(left, right);
}

StdExt::String operator+(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return StdExt::StringHelper::add(left.view(), right);
}

/////////////////////////////////////

bool operator<(const char* left, const StdExt::String& right)
{
	return right > left;
}

bool operator<(const std::string& left, const StdExt::String& right)
{
	return right > left;
}

bool operator<(const std::string_view& left, const StdExt::String& right)
{
	return right > left;
}

bool operator<(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return right > left.view();
}

/////////////////////////////////////

bool operator<=(const char* left, const StdExt::String& right)
{
	return right >= left;
}

bool operator<=(const std::string& left, const StdExt::String& right)
{
	return right >= left;
}

bool operator<=(const std::string_view& left, const StdExt::String& right)
{
	return right >= left;
}

bool operator<=(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return right >= left.view();
}

/////////////////////////////////////

bool operator==(const char* left, const StdExt::String& right)
{
	return right == left;
}

bool operator==(const std::string& left, const StdExt::String& right)
{
	return right == left;
}

bool operator==(const std::string_view& left, const StdExt::String& right)
{
	return right == left;
}

bool operator==(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return right == left.view();
}

/////////////////////////////////////

bool operator>=(const char* left, const StdExt::String& right)
{
	return right <= left;
}

bool operator>=(const std::string& left, const StdExt::String& right)
{
	return right <= left;
}

bool operator>=(const std::string_view& left, const StdExt::String& right)
{
	return right <= left;
}

bool operator>=(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return right <= left.view();
}

/////////////////////////////////////

bool operator>(const char* left, const StdExt::String& right)
{
	return right < left;
}

bool operator>(const std::string& left, const StdExt::String& right)
{
	return right < left;
}

bool operator>(const std::string_view& left, const StdExt::String& right)
{
	return right < left;
}

bool operator>(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return right < left.view();
}