#ifndef _STD_EXT_CONST_STRING_H_
#define _STD_EXT_CONST_STRING_H_

#include "StdExt.h"

#include <memory>
#include <string>

namespace StdExt
{
	class STD_EXT_EXPORT ConstString
	{
		friend class ConstStringUtility;

	public:
		ConstString(char* cstr);
		ConstString(std::string&& str);
		ConstString(const std::string& str);
		ConstString(const ConstString& other);

		virtual ~ConstString();

		ConstString& operator =(const ConstString& other);
		bool operator <(const ConstString& other) const;

		operator const std::string&() const;

	private:
		std::shared_ptr<std::string> mSharedString;
	};
}

#endif // !_STD_EXT_CONST_STRING_H_
