#ifndef _STD_EXT_CONST_STRING_H_
#define _STD_EXT_CONST_STRING_H_

#include "StdExt.h"

#include <functional>
#include <memory>
#include <string>

namespace StdExt
{
	class STD_EXT_EXPORT ConstString
	{
		friend class ConstStringUtility;

	public:
		static const ConstString Empty;

		ConstString();
		ConstString(char* cstr);
		ConstString(std::string&& str);
		ConstString(const std::string& str);
		ConstString(const ConstString& other);

		virtual ~ConstString();

		ConstString& operator =(const ConstString& other);

		bool operator ==(const ConstString& other) const;
		bool operator <(const ConstString& other) const;

		operator const std::string&() const;

	private:
		std::shared_ptr<std::string> mSharedString;
	};
}

namespace std
{
	template<>
	struct hash<StdExt::ConstString>
	{
		size_t operator()(const StdExt::ConstString& str) const noexcept
		{
			return hash<string>{}(str);
		}
	};
}

#endif // !_STD_EXT_CONST_STRING_H_
