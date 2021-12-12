#if defined(_MSC_VER) && !defined(_SCL_SECURE_NO_WARNINGS)
#	define _SCL_SECURE_NO_WARNINGS
#endif

#include <StdExt/String.h>
#include <StdExt/Number.h>

#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Streams/ByteStream.h>

#include <cassert>

namespace StdExt
{

	String String::join(const String* strings, size_t count, std::string_view glue)
	{
		size_t length = 0;

		for (size_t i = 0; i < count; ++i)
			length += strings[i].length();

		length += (count - 1) * glue.length();

		MemoryReference memory(length);

		char* start = (char*)memory.data();
		size_t index = 0;

		for (size_t i = 0; i < count; ++i)
		{
			strings[i].copy(&start[index], strings[i].length());
			index += strings[i].length();

			if (i != count - 1)
			{
				glue.copy(&start[index], glue.length());
				index += glue.length();
			}
		}

		start[length] = '\0';

		return String(std::move(memory));
	}

	String String::join(const std::vector<String>& strings, std::string_view glue)
	{
		return join(&strings[0], strings.size(), glue);
	}

	String String::join(const std::span<String>& strings, std::string_view glue)
	{
		return join(&strings[0], strings.size(), glue);
	}

	constexpr String::String() noexcept
		: mIsLiteral(false)
	{
		mView = std::string_view(&mSmallMemory[0], 0);
		mSmallMemory[0] = ('\0');
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
		: String()
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
			mHeapReference = std::move(mem);
			mView = std::string_view((const char*)mHeapReference.data(), mHeapReference.size());
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
			mHeapReference = mem;
			mView = std::string_view((const char*)mHeapReference.data(), mHeapReference.size());
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

	bool String::isLocal() const
	{
		return (mView.data() == mSmallMemory.data() || mIsLiteral);
	}

	bool String::isOnHeap() const
	{
		return (!mHeapReference.isNull());
	}

	void String::moveFrom(String&& other)
	{
		if (other.isLocal())
		{
			memcpy(mSmallMemory.data(), other.mView.data(), other.mView.size());
			mSmallMemory[other.mView.size()] = '\0';

			mView = std::string_view(mSmallMemory.data(), other.mView.size());
			mHeapReference.makeNull();
		}
		else
		{
			mView = other.mView;
			mIsLiteral = other.mIsLiteral;
			mHeapReference = std::move(other.mHeapReference);
		}

		other.mSmallMemory[0] = '\0';
		other.mView = std::string_view(other.mSmallMemory.data(), 0);
		other.mIsLiteral = false;
	}

	void String::copyFrom(const String& other)
	{
		if (other.isLocal())
		{
			memcpy(mSmallMemory.data(), other.mView.data(), other.mView.size());
			mSmallMemory[other.mView.size()] = '\0';

			mView = std::string_view(mSmallMemory.data(), other.mView.size());
			mHeapReference.makeNull();
		}
		else
		{
			mView = other.mView;
			mIsLiteral = other.mIsLiteral;
			mHeapReference = other.mHeapReference;
		}
	}

	void String::copyFrom(const std::string_view& view)
	{
		mIsLiteral = false;

		if (view.size() <= SmallSize)
		{
			mHeapReference.makeNull();
			memcpy(mSmallMemory.data(), view.data(), view.size());
			mSmallMemory[view.size()] = '\0';

			mView = std::string_view(mSmallMemory.data(), view.size());
		}
		else
		{
			mHeapReference = MemoryReference(view.size() + 1);
			char* heapBegin = (char*)mHeapReference.data();

			memcpy(heapBegin, view.data(), view.size());
			heapBegin[view.size()] = '\0';
			
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
		mHeapReference.makeNull();

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
			ret.mHeapReference = MemoryReference(combinedSize + 1);
			outMemory = (char*)ret.mHeapReference.data();
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

			mHeapReference = memory;
			mView = std::string_view((char*)mHeapReference.data(), combinedSize);
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
			ret.mHeapReference = mHeapReference;

			return ret;
		}
	}

	size_t String::find(const String& str, size_t pos) const
	{
		return mView.find(str.mView, pos);
	}

	size_t String::find(const StringLiteral& str, size_t pos) const
	{
		return mView.find(str.mView, pos);
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

	size_t String::rfind(const String& str, size_t pos) const
	{
		return mView.rfind(str.mView, pos);
	}

	size_t String::rfind(const StringLiteral& str, size_t pos) const
	{
		return mView.rfind(str.mView, pos);
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

	size_t String::find_first_of(const String& str, size_t pos) const
	{
		return mView.find_first_of(str.mView, pos);
	}

	size_t String::find_first_of(const StringLiteral& str, size_t pos) const
	{
		return mView.find_first_of(str.mView, pos);
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

	size_t String::find_last_of(const String& str, size_t pos) const
	{
		return mView.find_last_of(str.mView, pos);
	}

	size_t String::find_last_of(const StringLiteral& str, size_t pos) const
	{
		return mView.find_last_of(str.mView, pos);
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

	size_t String::find_first_not_of(const String& str, size_t pos) const
	{
		return mView.find_first_not_of(str.mView, pos);
	}

	size_t String::find_first_not_of(const StringLiteral& str, size_t pos) const
	{
		return mView.find_first_not_of(str.mView, pos);
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

	size_t String::find_last_not_of(const String& str, size_t pos) const
	{
		return mView.find_last_not_of(str.mView, pos);
	}

	size_t String::find_last_not_of(const StringLiteral& str, size_t pos) const
	{
		return mView.find_last_not_of(str.mView, pos);
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

	std::vector<String> String::split(const String& str, bool keepEmpty) const
	{
		return split(str.mView, keepEmpty);
	}

	std::vector<String> String::split(const StringLiteral& str, bool keepEmpty) const
	{
		return split(str.mView, keepEmpty);
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

	String String::trim()
	{
		constexpr StringLiteral Whitespace(" \t\r\n\v\f");

		size_t start = find_first_not_of(Whitespace);
		size_t end = find_last_not_of(Whitespace);

		if (npos == start || npos == end)
			return String();

		return substr(start, end - start);
	}

	bool String::isNullTerminated() const
	{
		const char* addrNullCheck = mView.data() + mView.size();
		return *addrNullCheck == '\0';
	}

	String String::getNullTerminated() const
	{
		if (isNullTerminated())
			return *this;
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

	void String::consolidate(const String& left, const String& right)
	{
		if  ( !left.isOnHeap() || !right.isOnHeap() || left.mHeapReference == right.mHeapReference )
			return;

		String* bigger = nullptr;
		String* smaller = nullptr;

		if (left.length() > right.length())
		{
			bigger = const_cast<String*>(&left);
			smaller = const_cast<String*>(&right);
		}
		else
		{
			bigger = const_cast<String*>(&right);
			smaller = const_cast<String*>(&left);
		}

		size_t start = bigger->find(*smaller);
		if (start != npos)
		{
			smaller->mView = bigger->mView.substr(start, smaller->length());
			smaller->mHeapReference = bigger->mHeapReference;
		}
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
				ret.mHeapReference = MemoryReference(combinedSize + 1);
				outMemory = (char*)ret.mHeapReference.data();
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

StdExt::String operator+(const StdExt::StringLiteral& left, const StdExt::StringLiteral& right)
{
	return StdExt::StringHelper::add(left.view(), right.view());
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

bool operator<(const StdExt::StringLiteral& left, const StdExt::StringLiteral& right)
{
	return left.view() < right.view();
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

bool operator<=(const StdExt::StringLiteral& left, const StdExt::StringLiteral& right)
{
	return left.view() <= right.view();
}

/////////////////////////////////////

bool operator==(const char* left, const StdExt::String& right)
{
	return right == left;
}

bool operator==(const std::string& left, const StdExt::String& right)
{
	return (0 == right.compare(left));
}

bool operator==(const std::string_view& left, const StdExt::String& right)
{
	return right == left;
}

bool operator==(const StdExt::StringLiteral& left, const StdExt::String& right)
{
	return right == left.view();
}

bool operator==(const StdExt::StringLiteral& left, const StdExt::StringLiteral& right)
{
	return left.view() == right.view();
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

bool operator>=(const StdExt::StringLiteral& left, const StdExt::StringLiteral& right)
{
	return left.view() >= right.view();
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

bool operator>(const StdExt::StringLiteral& left, const StdExt::StringLiteral& right)
{
	return left.view() > right.view();
}

namespace StdExt::Serialize::Binary
{
	template<>
	void write<std::string_view>(ByteStream* stream, const std::string_view& val)
	{
		uint32_t length = StdExt::Number::convert<uint32_t>(val.length());

		write<uint32_t>(stream, length);
		stream->writeRaw(val.data(), length);
	}

	template<>
	void read<StdExt::String>(ByteStream* stream, StdExt::String* out)
	{
		uint32_t length = read<uint32_t>(stream);

		if (length <= StdExt::String::SmallSize)
		{
			char buffer[StdExt::String::SmallSize];
			stream->readRaw(buffer, length);

			*out = StdExt::String(buffer, length);
		}
		else
		{
			StdExt::MemoryReference memRef(length + 1);
			stream->readRaw(memRef.data(), length);

			((char*)memRef.data())[length] = 0;

			*out = StdExt::String(std::move(memRef));
		}
	}

	template<>
	void write<StdExt::String>(ByteStream* stream, const StdExt::String& val)
	{
		write<std::string_view>(stream, val.view());
	}

	template<>
	void write<StdExt::StringLiteral>(ByteStream* stream, const StdExt::StringLiteral& val)
	{
		write<std::string_view>(stream, val.view());
	}

}
