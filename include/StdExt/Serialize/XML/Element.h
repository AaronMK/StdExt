#ifndef _STD_EXT_SERIALIZE_XML_ELEMENT_H_
#define _STD_EXT_SERIALIZE_XML_ELEMENT_H_

#include "../Serialize.h"
#include "../Exceptions.h"

#include "../../String.h"
#include "../../InPlace.h"

#include "../Text/Text.h"

#include <memory>
#include <functional>

namespace StdExt::Serialize::XML
{
	class ElementInternal;

	class STD_EXT_EXPORT Element final
	{
	public:
		static Element openFile(const StdExt::String& path);
		static Element parse(const StdExt::String& elmText);

		Element();
		Element(const StdExt::String& name);

		~Element();

		StdExt::String name() const;
		void setName(const StdExt::String& text);

		StdExt::String text() const;
		void setText(const StdExt::String& text);

		template<typename T>
		void setTextValue(const T& val)
		{
			setText(Text::write<T>(val));
		}

		template<typename T>
		T getTextValue() const
		{
			return Text::read<T>(text());
		}

		template<typename T>
		Element addChild(const StdExt::String& name, const T& value)
		{
			Element ret = addChildElement(name);
			write<T>(ret, value);

			return ret;
		}

		template<typename T>
		Element addChildVector(const StdExt::String& name, const std::vector<T>& value)
		{
			Element ret = addChildElement(name);
			writeVector<T>(ret, value);

			return ret;
		}

		template<typename T>
		T getChild(const StdExt::String& name) const
		{
			return read<T>(getChildElement(name));
		}

		template<typename T>
		void getChild(const StdExt::String& name, T* out) const
		{
			Element child = getChildElement(name);
			read<T>(child, out);
		}

		template<typename T>
		bool getChildVector(const StdExt::String& name, std::vector<T>& out) const
		{
			Element child = getChildElement(name);

			if (child.isValid())
			{
				out = readVector<T>(child);
				return true;
			}

			return false;
		}

		template<typename T>
		void setAttribute(const StdExt::String& name, const T& value)
		{
			setAttributeText(name, Text::write<T>(value));
		}

		template<typename T>
		bool getAttribute(const StdExt::String& name, T* out) const
		{
			StdExt::String strAttr;

			if ( getAttributeText(name, strAttr) )
			{
				*out = read<T>(strAttr);
				return true;
			}

			return false;
		}

		template<typename T>
		T getAttribute(const StdExt::String& name) const
		{
			T out;
			getAttribute(name, &out);

			return out;
		}

		Element addChildElement(const StdExt::String& name);
		Element getChildElement(const StdExt::String& name) const;
		
		void iterateChildren(const std::function<void(const Element&)>& func) const;

		StdExt::String toString() const;
		void save(const StdExt::String& path);

		bool isValid() const;

		StdExt::InPlace<ElementInternal, 64, true> mInternal;

	private:
		Element(ElementInternal&& internal);

		void setAttributeText(const StdExt::String& name, const StdExt::String& value);
		bool getAttributeText(const StdExt::String& name, StdExt::String& out) const;
	};
}

#endif // !_STD_EXT_SERIALIZE_XML_ELEMENT_H_