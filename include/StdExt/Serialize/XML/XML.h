#ifndef _STD_EXT_SERIALIZE_XML_H_
#define _STD_EXT_SERIALIZE_XML_H_

#include "Element.h"

#include "../../Number.h"

namespace StdExt::Serialize::XML
{
	template<typename T>
	void read(const Element& element, T* out) = delete;
	
	template<typename T>
	void write(Element& element, const T& val) = delete;

	template<Arithmetic T>
	void read(const Element& element, T* out)
	{
		*out = Number::parse(element.text()).value<T>();
	}
	
	template<Arithmetic T>
	void write(Element& element, const T& val)
	{
		element.setText(Number(val).toString());
	}

	template<>
	STD_EXT_EXPORT void read<StdExt::String>(const Element& element, StdExt::String* out);

	template<>
	STD_EXT_EXPORT void write<StdExt::String>(Element& element, const StdExt::String& val);

	template<>
	STD_EXT_EXPORT void read<std::string>(const Element& element, std::string* out);

	template<>
	STD_EXT_EXPORT void write<std::string>(Element& element, const std::string& val);

	template<>
	STD_EXT_EXPORT void read<bool>(const Element& element, bool* out);

	template<>
	STD_EXT_EXPORT void write<bool>(Element& element, const bool& val);

	template<Defaultable T>
	T read(const Element& element)
	{
		T ret;
		read<T>(element, &ret);

		return ret;
	}
}

#endif // _STD_EXT_SERIALIZE_XML_H_