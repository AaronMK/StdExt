#include "ElementInternal.h"

#include <functional>

using namespace std;
using namespace tinyxml2;

namespace StdExt::Serialize::XML
{
	ElementInternal::ElementInternal()
		: ElementInternal( StringLiteral("") )
	{
	}

	ElementInternal::ElementInternal(const StdExt::String& name)
	{
		mDocument = std::make_shared<XMLDocument>();
		
		mElement = mDocument->NewElement(name.getNullTerminated().data());
		mDocument->InsertEndChild(mElement);
	}

	ElementInternal::ElementInternal(const shared_ptr<XMLDocument>& otherDoc, tinyxml2::XMLElement* element)
		: mDocument(otherDoc), mElement(element)
	{
	}

	ElementInternal ElementInternal::addChild(const StdExt::String& name)
	{
		XMLElement* childElement = mDocument->NewElement(
			name.getNullTerminated().data()
		);
		mElement->InsertEndChild(childElement);

		return ElementInternal(mDocument, childElement);
	}

	ElementInternal ElementInternal::getChild(const StdExt::String& name) const
	{
		XMLElement* child = mElement->FirstChildElement(
			name.getNullTerminated().data()
		);

		if (nullptr == child)
			return ElementInternal();
		else
			return ElementInternal(mDocument, child);
	}

	StdExt::String ElementInternal::name() const
	{
		return mElement->Name();
	}

	void ElementInternal::setName(const StdExt::String& name)
	{
		mElement->SetName(name.getNullTerminated().data());
	}

	StdExt::String ElementInternal::text() const
	{
		return mElement->GetText();
	}

	void ElementInternal::setText(const char* text)
	{
		mElement->SetText(text);
	}

	void ElementInternal::setText(const std::string& text)
	{
		mElement->SetText(text.c_str());
	}

	void ElementInternal::setText(const StdExt::String& text)
	{
		mElement->SetText(text.getNullTerminated().data());
	}

	void ElementInternal::setText(const std::string_view& text)
	{
		mElement->SetText(StdExt::String(text).data());
	}

	void ElementInternal::setAttribute(const StdExt::String& name, const StdExt::String& value)
	{
		mElement->SetAttribute(
			name.getNullTerminated().data(),
			value.getNullTerminated().data()
		);
	}

	bool ElementInternal::getAttribute(const StdExt::String& name, StdExt::String& out) const
	{
		const char* attr = mElement->Attribute(name.getNullTerminated().data());

		if (nullptr != attr)
		{
			out = attr;
			return true;
		}

		return false;
	}

	bool ElementInternal::isValid() const
	{
		return (mDocument && mElement);
	}
}