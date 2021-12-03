#include <StdExt/Serialize/CodeGen/Field.h>

namespace StdExt::Serialize
{
	namespace CodeGen
	{
		Field::Field(const StdExt::String& pName)
			: mName(pName)
		{
			isMutable = false;
		}

		Field::~Field()
		{

		}

		StdExt::String Field::name()
		{
			return mName;
		}

		Type* Field::type()
		{
			return nullptr;
		}
	}
}