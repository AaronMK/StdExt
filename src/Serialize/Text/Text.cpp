#include <StdExt/Serialize/Text/Text.h>
#include <StdExt/Serialize/Exceptions.h>

#include <sstream>

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
		if (string.compare("true") == 0)
			*out = true;
		else if (string.compare("false") == 0)
			*out = false;
		else
			throw FormatException("Text for bool must either be 'true' or 'false'.");
	}

	template<>
	String write<bool>(const bool& val)
	{
		return val ? StringLiteral("true") : StringLiteral("false");
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