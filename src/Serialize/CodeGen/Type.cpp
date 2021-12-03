#include <StdExt/Serialize/CodeGen/Type.h>

#include <StdExt/Serialize/XML/XML.h>

#include <StdExt/Serialize/Exceptions.h>

namespace StdExt::Serialize::CodeGen
{
	class TypeShared final
	{
	public:
		TypeShared();
		~TypeShared();

		TypeShared* BaseType;
	};

	////////////////////////////////

	Type::Type(const StdExt::String& name, Namespace& namesapce)
	{
	}

	Type::Type(const StdExt::String& name)
	{
		mShared = std::make_shared<TypeShared>();
	}

	Type::~Type()
	{
	}

	void Type::setBaseType(const Type& type)
	{
	}

	void Type::addField(const Type& type, const StdExt::String& name)
	{
	}

	//////////////////////////////////

	TypeShared::TypeShared()
	{
	}

	TypeShared::~TypeShared()
	{
	}
}

namespace StdExt::Serialize
{
	using StringLiteral = StdExt::StringLiteral;
	using String = StdExt::String;

	using CoreType = Serialize::CodeGen::CoreType;

	namespace Text
	{
		constexpr StringLiteral sl_NONE("NONE");
		constexpr StringLiteral sl_INT_8("INT_8");
		constexpr StringLiteral sl_UINT_8("UINT_8");
		constexpr StringLiteral sl_INT_16("INT_16");
		constexpr StringLiteral sl_UINT_16("UINT_16");
		constexpr StringLiteral sl_INT_32("INT_32");
		constexpr StringLiteral sl_UINT_32("UINT_32");
		constexpr StringLiteral sl_INT_64("INT_64");
		constexpr StringLiteral sl_UINT_64("UINT_64");
		constexpr StringLiteral sl_FLOAT_32("FLOAT_32");
		constexpr StringLiteral sl_FLOAT_64("FLOAT_64");
		constexpr StringLiteral sl_STRING("STRING");
		constexpr StringLiteral sl_BYTE_ARRAY("BYTE_ARRAY");

		template<>
		void read<CoreType>(const String& string, CoreType* out)
		{
			if (string == sl_NONE)
				*out = CoreType::NONE;
			else if (string == sl_INT_8)
				*out = CoreType::INT_8;
			else if (string == sl_UINT_8)
				*out = CoreType::UINT_8;
			else if (string == sl_INT_16)
				*out = CoreType::INT_16;
			else if (string == sl_UINT_16)
				*out = CoreType::UINT_16;
			else if (string == sl_INT_32)
				*out = CoreType::INT_32;
			else if (string == sl_UINT_32)
				*out = CoreType::UINT_32;
			else if (string == sl_INT_64)
				*out = CoreType::INT_64;
			else if (string == sl_UINT_64)
				*out = CoreType::UINT_64;
			else if (string == sl_FLOAT_32)
				*out = CoreType::FLOAT_32;
			else if (string == sl_FLOAT_64)
				*out = CoreType::FLOAT_64;
			else if (string == sl_STRING)
				*out = CoreType::STRING;
			else if (string == sl_BYTE_ARRAY)
				*out = CoreType::BYTE_ARRAY;
			else
				throw FormatException::ForType<CoreType>();
		}

		template<>
		String write<CoreType>(const CoreType& val)
		{
			switch (val)
			{
			case CoreType::NONE:
				return sl_NONE;
			case CoreType::INT_8:
				return sl_INT_8;
			case CoreType::UINT_8:
				return sl_UINT_8;
			case CoreType::INT_16:
				return sl_INT_16;
			case CoreType::UINT_16:
				return sl_UINT_16;
			case CoreType::INT_32:
				return sl_INT_32;
			case CoreType::UINT_32:
				return sl_UINT_32;
			case CoreType::INT_64:
				return sl_INT_64;
			case CoreType::UINT_64:
				return sl_UINT_64;
			case CoreType::FLOAT_32:
				return sl_FLOAT_32;
			case CoreType::FLOAT_64:
				return sl_FLOAT_64;
			case CoreType::STRING:
				return sl_STRING;
			case CoreType::BYTE_ARRAY:
				return sl_BYTE_ARRAY;
			default:
				throw OutOfBounds("Attempted to write unsupported CoreType.");
			}
		}
	}

	namespace XML
	{
		constexpr StringLiteral AttribName("CoreType");

		template<>
		void read(const Element& element, CoreType* out)
		{
			*out = element.getAttribute<CoreType>(AttribName);
		}

		template<>
		void write(Element& element, const CoreType& val)
		{
			element.setAttribute(AttribName, val);
		}
	}
}