#ifndef _STD_EXT_TYPE_INFO_H_
#define _STD_EXT_TYPE_INFO_H_

#include "Concepts.h"
#include "Tags.h"
#include "Utility.h"

namespace StdExt
{
	template<typename T = BASE>
	class TypeInfo;

	template<>
	class TypeInfo<BASE>
	{
	public:
		virtual bool isClass() const = 0;
		virtual bool isEmpty() const = 0;
		virtual bool isFinal() const = 0;
		virtual bool isAbstract() const = 0;
		virtual bool isPolymorphic() const = 0;

		virtual bool isScaler() const = 0;
		virtual bool isPointer() const = 0;
		virtual bool isReference() const = 0;
		virtual bool isConst() const = 0;
		
		virtual bool isDefaultConstructible() const = 0;
		virtual bool isTriviallyConstructable() const = 0;
		virtual bool isTriviallyDestructable() const = 0;
		virtual bool isTriviallyMovable() const = 0;
		virtual bool isTriviallyCopyable() const = 0;

		virtual size_t size() const = 0;
		virtual size_t alignment() const = 0;

		virtual std::type_index stdIndex() const = 0;
		virtual const std::type_info& stdInfo() const = 0;

		virtual std::type_index indexTypeFunctions() const = 0;
	};

	template<typename T>
	class TypeInfo : public TypeInfo<BASE>
	{
	public:
		bool isClass() const override { return std::is_class_v<T>; }
		bool isEmpty() const override { return std::is_empty_v<T>; }
		bool isFinal() const override { return std::is_final_v<T>; }
		bool isAbstract() const override { return std::is_abstract_v<T>; }
		bool isPolymorphic() const override { return std::is_polymorphic_v<T>; }

		bool isScaler() const override { return Scalar<T>; }
		bool isPointer() const override { return std::is_pointer_v<T>; }
		bool isReference() const override { return std::is_reference_v<T>; }
		bool isConst() const override { return std::is_const_v<T>; }

		bool isDefaultConstructible() const override { return DefaultConstructible<T>; }
		bool isTriviallyConstructable() const override { return std::is_trivially_constructible_v<T>; }
		bool isTriviallyDestructable() const override { return std::is_trivially_destructible_v<T>; }
		bool isTriviallyMovable() const override { return std::is_trivially_move_constructible_v<T>; }
		bool isTriviallyCopyable() const override { return std::is_trivially_copy_constructible_v<T>; }

		size_t size() const override
		{
			if constexpr (std::is_void_v<T>)
				return 0;
			else
				return sizeof(T);
		}

		size_t alignment() const override
		{
			if constexpr ( std::is_void_v<T> )
				return 0;
			else
				return alignof(T); 
		}

		std::type_index stdIndex() const override { return std::type_index(typeid(T)); }
		const std::type_info& stdInfo() const override { return typeid(T); }

		virtual std::type_index indexTypeFunctions() const override
		{
			return std::type_index( typeid(T) );
		}
	};

	using TypeFunctions = VTable<TypeInfo<BASE>>;

	template<typename T>
	static TypeFunctions TypeFunctionsOf()
	{
		TypeFunctions ret;
		ret.set<TypeInfo<T>>();

		return ret;
	}
}

#endif // !_STD_EXT_TYPE_INFO_H_