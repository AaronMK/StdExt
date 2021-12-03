#ifndef _STD_EXT_SERIALIZE_CODE_GEN_DOCUMENT_H_
#define _STD_EXT_SERIALIZE_CODE_GEN_DOCUMENT_H_

#include "Namespace.h"
#include "Enumeration.h"

#include "Exceptions.h"

#include <StdExt/InPlace.h>
#include <StdExt/String.h>

#include <string>
#include <memory>

namespace StdExt::Serialize::CodeGen
{
	class Type;
	class DocumentInternal;
	class NamespaceInternal;
	class EnumerationInternal;

	class STD_EXT_EXPORT Document
	{

	public:
		struct CppProps
		{
			StdExt::String IncludeGuard;
			StdExt::String ExportPreProcessDefine;
		};

		Document(const Document&) = delete;
		Document& operator=(const Document&) = delete;

		Document(Document&&) = default;
		Document& operator=(Document&&) = default;

		Document();
		Document(std::string_view filePath);

		~Document();

		Namespace getNamespace(const StdExt::String& name);
		Enumeration getEnumeration(const StdExt::String& name);

		CppProps CPP;

	private:
		std::shared_ptr<DocumentInternal> mDocInternal;

		std::map<StdExt::String, std::shared_ptr<NamespaceInternal>> mNamespaces;
		std::map<StdExt::String, std::shared_ptr<EnumerationInternal>> mEnumerations;
	};
}

namespace StdExt::Serialize
{
	namespace XML
	{
		template<>
		STD_EXT_EXPORT void read(const Element& element, Serialize::CodeGen::Document::CppProps* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Serialize::CodeGen::Document::CppProps& val);
	}
}

#endif // !_STD_EXT_SERIALIZE_CODE_GEN_DOCUMENT_H_