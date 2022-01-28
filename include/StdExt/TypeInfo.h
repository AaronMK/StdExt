#ifndef _STD_EXT_TYPE_INFO_H_
#define _STD_EXT_TYPE_INFO_H_

#include "Concepts.h"
#include "Utility.h"

namespace StdExt
{
	namespace Internal
	{
		class TypeInfoFunctions
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
			
			virtual bool isDefaultConstructable() const = 0;
			virtual bool isTriviallyConstructable() const = 0;
			virtual bool isTriviallyDestructable() const = 0;
			virtual bool isTriviallyMovable() const = 0;
			virtual bool isTriviallyCopyable() const = 0;

			virtual size_t size() const = 0;
			virtual size_t alignment() const = 0;

			virtual std::type_index indexTypeFunctions() const = 0;
		};

		template<typename T>
		class TypeInfoFunctionsFor final : public TypeInfoFunctions
		{
		public:
			bool isClass() const override { return std::is_class_v<T>; };
			bool isEmpty() const override { return std::is_empty_v<T>; };
			bool isFinal() const override { return std::is_final_v<T>; };
			bool isAbstract() const override { return std::is_abstract_v<T>; };
			bool isPolymorphic() const override { return std::is_polymorphic_v<T>; };

			bool isScaler() const override { return Scaler<T>; };
			bool isPointer() const override { return std::is_pointer_v<T>; };
			bool isReference() const override { return std::is_reference_v<T>; };
			bool isConst() const override { return std::is_const_v<T>; };

			bool isDefaultConstructable() const override { return DefaultConstructable<T>; };
			bool isTriviallyConstructable() const override { return std::is_trivially_constructible_v<T>; };
			bool isTriviallyDestructable() const override { return std::is_trivially_destructible_v<T>; };
			bool isTriviallyMovable() const override { return std::is_trivially_move_constructible_v<T>; };
			bool isTriviallyCopyable() const override { return std::is_trivially_copy_constructible_v<T>; };

			size_t size() const override { return sizeof(T); }
			size_t alignment() const override { return alignof(T); };

			virtual std::type_index indexTypeFunctions() const override
			{
				return std::type_index( typeid(TypeInfoFunctionsFor<T>) );
			}
		};
	}

	class TypeInfo final
	{
	private:
		VTable<Internal::TypeInfoFunctions> mInfo;

	public:

		template<typename T>
		static TypeInfo of()
		{
			TypeInfo ret;
			ret.mInfo.set< Internal::TypeInfoFunctionsFor<T> >();

			return ret;
		}

		bool operator==(const TypeInfo& other) const
		{
			return mInfo->indexTypeFunctions() == other.mInfo->indexTypeFunctions();
		}

		bool operator!=(const TypeInfo& other) const
		{
			return mInfo->indexTypeFunctions() != other.mInfo->indexTypeFunctions();
		}

		bool isClass() const { return mInfo->isClass(); };
		bool isEmpty() const { return mInfo->isEmpty(); };
		bool isFinal() const { return mInfo->isFinal(); };
		bool isAbstract() const { return mInfo->isAbstract(); };
		bool isPolymorphic() const { return mInfo->isPolymorphic(); };

		bool isScaler() const { return mInfo->isScaler(); };
		bool isPointer() const { return mInfo->isPointer(); };
		bool isReference() const { return mInfo->isReference(); };
		bool isConst() const { return mInfo->isConst(); };

		bool isDefaultConstructable() const { return mInfo->isDefaultConstructable(); };
		bool isTriviallyConstructable() const { return mInfo->isTriviallyConstructable(); };
		bool isTriviallyDestructable() const { return mInfo->isTriviallyDestructable(); };
		bool isTriviallyMovable() const { return mInfo->isTriviallyMovable(); };
		bool isTriviallyCopyable() const { return mInfo->isTriviallyCopyable(); };

		size_t size() const { return mInfo->size(); }
		size_t alignment() const { return mInfo->alignment(); };
	};

	template<typename T>
	static TypeInfo typeInfo()
	{
		return TypeInfo::of<T>();
	}
}

#endif // !_STD_EXT_TYPE_INFO_H_