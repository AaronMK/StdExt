#ifndef _STD_EXT_SERIALIZE_CODE_GEN_FIELD_H_
#define _STD_EXT_SERIALIZE_CODE_GEN_FIELD_H_

#include "Type.h"
#include "Documentation.h"

#include <string>
#include <memory>

namespace StdExt::Serialize
{
	namespace CodeGen
	{
		class TypeInternal;

		class STD_EXT_EXPORT Field
		{
		public:
			Field(const StdExt::String& pName);
			virtual ~Field();

			StdExt::String name();
			Type* type();

			bool isMutable;
			Documentation Docs;

		private:
			StdExt::String mName;
			std::shared_ptr<TypeInternal> mType;
		};
	}
}

#endif // !_STD_EXT_SERIALIZE_CODE_GEN_FIELD_H_