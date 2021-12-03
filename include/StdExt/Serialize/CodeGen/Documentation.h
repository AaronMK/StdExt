#ifndef _STD_EXT_SERIALIZE_CODE_GEN_DOCUMENTATION_H_
#define _STD_EXT_SERIALIZE_CODE_GEN_DOCUMENTATION_H_

#include "../../StdExt.h"

#include <StdExt/String.h>

#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Serialize/XML/XML.h>

namespace StdExt::Serialize
{
	namespace CodeGen
	{
		struct Documentation
		{
			bool Internal = false;
			StdExt::String Brief;
			StdExt::String Detailed;

			bool isEmpty() const
			{
				return ((Brief.length() + Detailed.length()) == 0);
			}
		};
	}

	namespace Binary
	{
		template<>
		STD_EXT_EXPORT void read(ByteStream* stream, CodeGen::Documentation* out);

		template<>
		STD_EXT_EXPORT void write(ByteStream* stream, const CodeGen::Documentation& val);
	}

	namespace XML
	{
		template<>
		STD_EXT_EXPORT void read(const Element& element, CodeGen::Documentation* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const CodeGen::Documentation& val);
	}
}

#endif // !_STD_EXT_SERIALIZE_CODE_GEN_DOCUMENTATION_H_