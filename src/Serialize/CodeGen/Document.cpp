#include <StdExt/Serialize/CodeGen/Document.h>

#include <StdExt/Serialize/CodeGen/Enumeration.h>
#include <StdExt/Serialize/CodeGen/Namespace.h>
#include <StdExt/Serialize/CodeGen/Type.h>

#include <StdExt/Serialize/XML/XML.h>

#include <StdExt/Exceptions.h>

#include "StringLiterals.h"
#include "NamespaceInternal.h"

using namespace std;
using namespace StdExt;

namespace StdExt::Serialize::CodeGen
{
	Document::Document()
	{
	}

	Document::Document(std::string_view filePath)
		: Document()
	{
		XML::Element root = XML::Element::openFile(filePath);

		root.iterateChildren(
			[this](const XML::Element& childElement)
			{
				if (childElement.name() == slNamespace)
				{
					String strNamespace = childElement.getAttribute<String>(slName);

					auto itr = mNamespaces.find(strNamespace);
					shared_ptr<NamespaceInternal> nsInternal;

					if (itr == mNamespaces.end())
					{
						nsInternal = make_shared<NamespaceInternal>();
						nsInternal->FullName = strNamespace;
						nsInternal->Name = strNamespace;

						mNamespaces[strNamespace] = nsInternal;
					}
					else
					{
						nsInternal = itr->second;
					}
				}
			}
		);
	}

	Document::~Document()
	{
	}

	Namespace Document::getNamespace(const StdExt::String& name)
	{
		auto itr = mNamespaces.find(name);
		shared_ptr<NamespaceInternal> nsInternal;

		if (itr == mNamespaces.end())
		{
			nsInternal = std::make_shared<NamespaceInternal>();
			nsInternal->Name = name;
			nsInternal->FullName = name;

			mNamespaces[name] = nsInternal;
		}
		else
		{
			nsInternal = (*itr).second;
		}

		Namespace nsReturn;
		nsReturn.mNamespaceInternal = nsInternal;
		nsReturn.mDocInternal = mDocInternal;

		return nsReturn;
	}

	Enumeration Document::getEnumeration(const StdExt::String& name)
	{
		throw StdExt::not_implemented();
	}
}

namespace StdExt::Serialize
{
	namespace XML
	{
		using StringLiteral = StdExt::StringLiteral;

		static const StringLiteral slIncludeGuard("IncludeGuard");
		static const StringLiteral slExportPreProcessDefine("ExportPreProcessDefine");

		template<>
		void read(const Element& element, Serialize::CodeGen::Document::CppProps* out)
		{
			element.getChild<StdExt::String>(slIncludeGuard, &out->IncludeGuard);
			element.getChild<StdExt::String>(slExportPreProcessDefine, &out->ExportPreProcessDefine);
			return STD_EXT_EXPORT void();
		}

		template<>
		void write(Element& element, const Serialize::CodeGen::Document::CppProps& val)
		{
			element.addChild<StdExt::String>(slIncludeGuard, val.IncludeGuard);
			element.addChild<StdExt::String>(slExportPreProcessDefine, val.ExportPreProcessDefine);
		}
	}
}