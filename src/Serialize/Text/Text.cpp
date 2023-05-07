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
		if ( string == String::literal(u8"true") )
			*out = true;
		else if ( string == String::literal(u8"false") )
			*out = false;
		else
			throw FormatException("Text for bool must either be 'true' or 'false'.");
	}

	template<>
	String write<bool>(const bool& val)
	{
		return val ? String::literal(u8"true") : String::literal(u8"false");
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