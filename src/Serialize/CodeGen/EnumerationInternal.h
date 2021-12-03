#ifndef _STD_EXT_SERIALIZE_CODEGEN_ENUMERATION_INTERNAL_H_
#define _STD_EXT_SERIALIZE_CODEGEN_ENUMERATION_INTERNAL_H_

#include <StdExt/Serialize/CodeGen/Type.h>

#include <StdExt/String>

#include <map>

namespace StdExt::Serialize::CodeGen
{
	class EnumerationInternal
	{
	public:
		CoreType UnderlyingType;
		std::map<int32_t, StdExt::String> Options;
	};
}

#endif // _STD_EXT_SERIALIZE_CODEGEN_ENUMERATION_INTERNAL_H_