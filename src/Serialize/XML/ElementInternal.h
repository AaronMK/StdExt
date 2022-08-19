#ifndef _SERIALIZE_XML_ELEMENT_INTERNAL_H_
#define _SERIALIZE_XML_ELEMENT_INTERNAL_H_

#include <StdExt/String.h>

#include "../TinyXml2/tinyxml2.h"

#include <memory>

namespace StdExt::Serialize::XML
{
	class ElementInternal
	{
	public:
		std::shared_ptr<tinyxml2::XMLDocument> mDocument;
		tinyxml2::XMLElement* mElement;

		ElementInternal();

		ElementInternal(const StdExt::String& name);
		ElementInternal(const std::shared_ptr<tinyxml2::XMLDocument>& otherDoc, tinyxml2::XMLElement* element);

		ElementInternal addChild(const StdExt::String& name);
		ElementInternal getChild(const StdExt::String& name) const;

		StdExt::String name() const;
		void setName(const StdExt::String& name);

		StdExt::String text() const;

		void setText(const char* text);
		void setText(const std::u8string& text);
		void setText(const StdExt::String& text);
		void setText(const std::string_view& text);

		void setAttribute(const StdExt::String& name, const StdExt::String& value);
		bool getAttribute(const StdExt::String& name, StdExt::String& out) const;

		bool isValid() const;
	};
}

#endif // !_SERIALIZE_XML_ELEMENT_INTERNAL_H_