#include <StdExt/Serialize/Text/Text.h>
#include <StdExt/Serialize/Exceptions.h>

#include <sstream>

#include <StdExt/Compare.h>

namespace StdExt::Serialize::Text
{

	template<>
	void read<Number>(const String& string, Number* out)
	{
		*out = Number::parse(string);
	}

	template<>
	StdExt::String write<Number>(const Number& val)
	{
		return val.toString();
	}

	template<>
	void read<bool>(const String& string, bool* out)
	{
		if ( compare(string, String::literal("true")) == 0)
			*out = true;
		else if ( compare(string, String::literal("false")) == 0)
			*out = false;
		else
			throw FormatException("Text for bool must either be 'true' or 'false'.");
	}

	template<>
	String write<bool>(const bool& val)
	{
		return val ? String::literal("true") : String::literal("false");
	}

	template<>
	void read<String>(const String& string, String* out)
	{
		*out = string;
	}

	template<>
	String write<String>(const String& val)
	{
		return val;
	}
}