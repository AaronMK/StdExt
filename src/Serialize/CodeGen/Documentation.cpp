#include <StdExt/Serialize/CodeGen/Documentation.h>

#include <StdExt/Serialize/XML/XML.h>

namespace StdExt::Serialize::Binary
{
	template<>
	void read<CodeGen::Documentation>(ByteStream* stream, CodeGen::Documentation *out)
	{
		read(stream, &out->Internal);
		read(stream, &out->Brief);
		read(stream, &out->Detailed);
	}

	template<>
	void write<CodeGen::Documentation>(ByteStream* stream, const CodeGen::Documentation &val)
	{
		write(stream, val.Internal);
		write(stream, val.Brief);
		write(stream, val.Detailed);
	}
}

namespace StdExt::Serialize::XML
{
	using StringLiteral = StdExt::StringLiteral;

	static const StringLiteral slInternal("Internal");
	static const StringLiteral slBrief("Brief");
	static const StringLiteral slDetailed("Detailed");

	template<>
	void read(const Element& element, CodeGen::Documentation* out)
	{
		element.getAttribute<bool>(slInternal, &out->Internal);
		element.getChild<StdExt::String>(slBrief, &out->Brief);
		element.getChild<StdExt::String>(slDetailed, &out->Detailed);
	}

	template<>
	void write(Element& element, const CodeGen::Documentation& val)
	{
		element.setAttribute(slInternal, val.Internal);
		element.addChild(slBrief, val.Brief);
		element.addChild(slDetailed, val.Detailed);
	}
}