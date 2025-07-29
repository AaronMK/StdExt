#include "ElementInternal.h"

#include <functional>

using namespace std;
using namespace tinyxml2;

namespace StdExt::Serialize::XML
{
	ElementInternal::ElementInternal()
		: ElementInternal( String::literal(u8"") )
	{
	}

	ElementInternal::ElementInternal(const StdExt::String& name)
	{
		mDocument = std::make_shared<XMLDocument>();
		
		mElement = mDocument->NewElement(
			access_as<const char*>(name.getNullTerminated().data())
		);
		mDocument->InsertEndChild(mElement);
	}

	ElementInternal::ElementInternal(const std::shared_ptr<tinyxml2::XMLDocument>& otherDoc, tinyxml2::XMLElement* element)
		: mDocument(otherDoc), mElement(element)
	{
	}

	ElementInternal ElementInternal::addChild(const StdExt::String& name)
	{
		XMLElement* childElement = mDocument->NewElement(
			access_as<const char*>(name.getNullTerminated().data())
		);
		mElement->InsertEndChild(childElement);

		return ElementInternal(mDocument, childElement);
	}

	ElementInternal ElementInternal::getChild(const StdExt::String& name) const
	{
		XMLElement* child = mElement->FirstChildElement(
			access_as<const char*>(name.getNullTerminated().data())
		);

		if (nullptr == child)
			return ElementInternal();
		else
			return ElementInternal(mDocument, child);
	}

	StdExt::String ElementInternal::name() const
	{
		return access_as<const char8_t*>(mElement->Name());
	}

	void ElementInternal::setName(const StdExt::String& name)
	{
		mElement->SetName(
			access_as<const char*>(name.getNullTerminated().data())
		);
	}

	StdExt::String ElementInternal::text() const
	{
		return access_as<const char8_t*>(mElement->GetText());
	}

	void ElementInternal::setText(const char* text)
	{
		mElement->SetText(text);
	}

	void ElementInternal::setText(const std::u8string& text)
	{
		mElement->SetText(text.c_str());
	}

	void ElementInternal::setText(const StdExt::String& text)
	{
		mElement->SetText(
			access_as<const char*>(
				text.getNullTerminated().data()
			)
		);
	}

	void ElementInternal::setText(const std::string_view& text)
	{
		auto str = convertString<char8_t>(
			StdExt::CString(text.data())
		);

		mElement->SetText(str.data());
	}

	void ElementInternal::setAttribute(const StdExt::String& name, const StdExt::String& value)
	{
		mElement->SetAttribute(
			access_as<const char*>(name.getNullTerminated().data()),
			access_as<const char*>(value.getNullTerminated().data())
		);
	}

	bool ElementInternal::getAttribute(const StdExt::String& name, StdExt::String& out) const
	{
		const char* attr = mElement->Attribute(
			access_as<const char*>(name.getNullTerminated().data())
		);

		if (nullptr != attr)
		{
			out = StdExt::String(access_as<const char8_t*>(attr));
			return true;
		}

		return false;
	}

	bool ElementInternal::isValid() const
	{
		return (mDocument && mElement);
	}
}