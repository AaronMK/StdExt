#ifndef _STD_EXT_SERIALIZE_CODE_GEN_TYPE_H_
#define _STD_EXT_SERIALIZE_CODE_GEN_TYPE_H_

#include "Namespace.h"
#include "Documentation.h"

#include <StdExt/InPlace.h>

#include <StdExt/Serialize/XML/Element.h>

#include <string>
#include <memory>

namespace StdExt::Serialize::CodeGen
{
	enum class CoreType
	{
		NONE,
		INT_8,
		UINT_8,
		INT_16,
		UINT_16,
		INT_32,
		UINT_32,
		INT_64,
		UINT_64,
		FLOAT_32,
		FLOAT_64,
		STRING,
		BYTE_ARRAY,
	};

	class TypeShared;
	
	class STD_EXT_EXPORT Type
	{
	public:
		Type(const StdExt::String& name, Namespace& namesapce);
		Type(const StdExt::String& name);

		virtual ~Type();

		void setBaseType(const Type& type);
		void addField(const Type& type, const StdExt::String& name);

		Documentation Docs;

	private:
		std::shared_ptr<TypeShared> mShared;
	};
}

namespace StdExt::Serialize
{
	namespace Text
	{
		template<>
		STD_EXT_EXPORT void read<Serialize::CodeGen::CoreType>(const StdExt::String& string, Serialize::CodeGen::CoreType* out);

		template<>
		STD_EXT_EXPORT StdExt::String write<Serialize::CodeGen::CoreType>(const Serialize::CodeGen::CoreType& val);
	}

	namespace XML
	{
		template<>
		STD_EXT_EXPORT void read(const Element& element, Serialize::CodeGen::CoreType* out);

		template<>
		STD_EXT_EXPORT void write(Element& element, const Serialize::CodeGen::CoreType& val);
	}
}

#endif // !_STD_EXT_SERIALIZE_CODE_GEN_TYPE_H_