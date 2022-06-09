#include <StdExt/Serialize/XML/XML.h>
#include <StdExt/Serialize/Exceptions.h>

#include <StdExt/Compare.h>

namespace StdExt::Serialize::XML
{
	template<>
	void read<StdExt::String>(const Element& element, StdExt::String* out)
	{
		*out = element.text();
	}

	template<>
	void write<StdExt::String>(Element& element, const StdExt::String& val)
	{
		element.setText(val);
	}

	template<>
	void read<std::string>(const Element& element, std::string* out)
	{
		*out = element.text().toStdString();
	}

	template<>
	void write<std::string>(Element& element, const std::string& val)
	{
		element.setText(StdExt::String(val));
	}

	template<>
	void read<bool>(const Element& element, bool* out)
	{
		auto string(element.text());

		if ( compare(string, String::literal("true")) == 0)
			*out = true;
		else if ( compare(string, String::literal("false")) == 0)
			*out = false;
		else
			throw FormatException("Text for bool must either be 'true' or 'false'.");
	}

	template<>
	void write<bool>(Element& element, const bool& val)
	{
		element.setText(val ? String::literal("true") : String::literal("false"));
	}
}