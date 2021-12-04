#include <StdExt/Serialize/XML/Element.h>

#include <StdExt/Streams/FileStream.h>
#include <StdExt/Collections/Vector.h>
#include <StdExt/Memory.h>

#include "ElementInternal.h"

using namespace StdExt;
using namespace tinyxml2;

using namespace StdExt;

namespace StdExt::Serialize::XML
{
	using FileStream = StdExt::Streams::FileStream;

	class TabbedPrinter : public XMLPrinter
	{
	protected:
		virtual void PrintSpace(int depth) override
		{
			Collections::Vector<char, 16> tabBuffer(depth + 1);
			memset(tabBuffer.data(), '\t', depth);
			tabBuffer[depth] = 0;

			Print(tabBuffer.data());
		}
	};

	Element Element::openFile(const String& path)
	{
		String nullTerminated = path.getNullTerminated();
		
		Element root;
		root.mInternal.setValue<ElementInternal>();
		root.mInternal->mDocument = std::make_shared<XMLDocument>();

		root.mInternal->mDocument->LoadFile(nullTerminated.data());
		root.mInternal->mElement = root.mInternal->mDocument->RootElement();

		return root;
	}

	Element Element::parse(const String& elmText)
	{
		Element root;
		root.mInternal.setValue<ElementInternal>();
		root.mInternal->mDocument = std::make_shared<XMLDocument>();

		root.mInternal->mDocument->Parse(elmText.data(), elmText.size());
		root.mInternal->mElement = root.mInternal->mDocument->RootElement();

		return root;
	}

	Element::Element()
	{
		mInternal.setValue<ElementInternal>();
	}

	Element::Element(const String& name)
	{
		String nullTerminated = name.getNullTerminated();
		mInternal.setValue<ElementInternal>(nullTerminated.data());
	}

	Element::~Element()
	{
	}

	Element::Element(ElementInternal&& elmInternal)
	{
		mInternal.setValue<ElementInternal>(std::move(elmInternal));
	}

	StdExt::String Element::name() const
	{
		return mInternal->name();
	}

	void Element::setName(const String& text)
	{
		mInternal->setName(text);
	}

	StdExt::String Element::text() const
	{
		return mInternal->text();
	}

	void Element::setText(const String& text)
	{
		mInternal->setText(text);
	}

	String Element::toString() const
	{
		TabbedPrinter printer;
		mInternal->mElement->Accept(&printer);

		return String(printer.CStr(), printer.CStrSize() - 1);
	}

	void Element::save(const String& path)
	{
		FileStream outFile;
		outFile.open(path, false);

		String strElm = toString();
		outFile.writeRaw(strElm.data(), Number(strElm.length()));
	}

	bool Element::isValid() const
	{
		return mInternal->isValid();
	}

	Element Element::addChildElement(const String& name)
	{
		return Element(
			mInternal->addChild(name)
		);
	}

	void Element::iterateChildren(const std::function<void(const Element&)>& func) const
	{
		if (!isValid())
			return;

		for (XMLElement* txElm = mInternal->mElement->FirstChildElement(); txElm != nullptr; txElm = txElm->NextSiblingElement())
		{
			Element elm;
			elm.mInternal->mDocument = mInternal->mDocument;
			elm.mInternal->mElement = txElm;

			func(elm);
		}
	}

	Element Element::getChildElement(const String& name) const
	{
		if (isValid())
		{
			ElementInternal childInternal = mInternal->getChild(name);
			return Element(std::move(childInternal));
		}
		else
		{
			return Element();
		}
	}
	/*
	template<>
	void Element::addAsChildElement(const StdExt::String& name, const StdExt::String& value)
	{
		Element child = addChildElement(name);
		child.setText(value);
	}

	template<>
	void Element::addAsChildElement(const StdExt::String& name, const std::string& value)
	{
		Element child = addChildElement(name);
		child.setText(StdExt::String(value));
	}
	*/

	void Element::setAttributeText(const String& name, const String& value)
	{
		mInternal->setAttribute(name, value);
	}

	bool Element::getAttributeText(const String& name, String& out) const
	{
		return mInternal->getAttribute(name, out);
	}
}