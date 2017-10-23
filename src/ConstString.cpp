#include <StdExt/ConstString.h>

#include <atomic>
#include <cassert>

using namespace std;

namespace StdExt
{
	class ConstStringUtility
	{
	public:
		static void consolidate(const ConstString* Left, const ConstString* Right)
		{
			ConstString* nonConstLeft = const_cast<ConstString*>(Left);
			ConstString* nonConstRight = const_cast<ConstString*>(Right);

			if (nonConstLeft->mSharedString.use_count() >= nonConstRight->mSharedString.use_count())
				atomic_store(&nonConstRight->mSharedString, nonConstLeft->mSharedString);
			else
				atomic_store(&nonConstLeft->mSharedString, nonConstRight->mSharedString);
		}
	};

	/////////////////////////////

	static const ConstString EmptyString("");

	ConstString::ConstString()
		: ConstString(EmptyString)
	{
	}

	ConstString::ConstString(char* cstr)
	{
		mSharedString = make_shared<string>(cstr);
	}

	ConstString::ConstString(std::string&& str)
	{
		mSharedString = make_shared<string>(move(str));
	}

	ConstString::ConstString(const std::string& str)
	{
		mSharedString = make_shared<string>(str);
	}

	ConstString::ConstString(const ConstString& other)
	{
		atomic_store(&mSharedString, other.mSharedString);
	}

	ConstString::~ConstString()
	{
	}

	ConstString& ConstString::operator=(const ConstString& other)
	{
		atomic_store(&mSharedString, other.mSharedString);
		return *this;
	}

	bool ConstString::operator <(const ConstString& other) const
	{
		int compResult = mSharedString->compare(*other.mSharedString);

		if (0 == compResult)
			ConstStringUtility::consolidate(this, &other);

		return (compResult < 1);
	}

	ConstString::operator const std::string&() const
	{
		return *mSharedString;
	}
}