#ifndef _STD_EXT_SERIALIZE_CODE_GEN_NAMESPACE_H_
#define _STD_EXT_SERIALIZE_CODE_GEN_NAMESPACE_H_

#include "Documentation.h"

#include <StdExt/String.h>

#include <map>
#include <memory>

namespace StdExt::Serialize::CodeGen
{
	class NamespaceInternal;
	class DocumentInternal;

	class STD_EXT_EXPORT Namespace
	{
		friend class Document;
		friend class NamespaceInternal;

	public:
		const StdExt::String& name() const;
		Namespace getNamespace(const StdExt::String& name);

		Documentation Docs;

	private:
		std::shared_ptr<NamespaceInternal> mNamespaceInternal;
		std::shared_ptr<DocumentInternal> mDocInternal;
	};
}

#endif // !_STD_EXT_SERIALIZE_CODE_GEN_NAMESPACE_H_