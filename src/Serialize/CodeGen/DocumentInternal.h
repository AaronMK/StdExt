#ifndef _STD_EXT_SERIALIZE_CODEGEN_DOCUMENT_INTERNAL_H_
#define _STD_EXT_SERIALIZE_CODEGEN_DOCUMENT_INTERNAL_H_

#include <StdExt/String.h>

#include <memory>
#include <map>

namespace StdExt::Serialize::CodeGen
{
	class DocumentInternal
	{
	public:
		std::map<StdExt::String, Namespace> mNamespaces;
		std::map<StdExt::String, Enumeration> mEnums;
		std::map<StdExt::String, Type> mTypes;
	};
}

#endif // _STD_EXT_SERIALIZE_CODEGEN_DOCUMENT_INTERNAL_H_