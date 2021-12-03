#ifndef _STD_EXT_SERIALIZE_CODE_GEN_ENUMERATION_H_
#define _STD_EXT_SERIALIZE_CODE_GEN_ENUMERATION_H_

#include "Type.h"
#include "Documentation.h"

#include <StdExt/Number.h>

#include <list>
#include <optional>

namespace StdExt::Serialize::CodeGen
{
	class STD_EXT_EXPORT Enumeration : public Type
	{
	public:
		struct Option
		{
			Documentation Docs;
			std::string Name;
			int32_t value;
		};

		Enumeration(const StdExt::String& name);
		virtual ~Enumeration();

		Documentation Docs;
		std::list<Option> Options;
	};
}


#endif // !_STD_EXT_SERIALIZE_CODE_GEN_ENUMERATION_H_