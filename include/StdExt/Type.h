#ifndef _STD_EXT_TYPE_H_
#define _STD_EXT_TYPE_H_

#include "Exceptions.h"
#include "InPlace.h"
#include "String.h"

#include <type_traits>

namespace StdExt
{
	class Type
	{
		class iTypeImp
		{
		public:
			virtual bool canDefaultConstruct() const = 0;

			virtual bool canCopy() const = 0;
			virtual bool canMove() const = 0;
			virtual bool canCopyAssign() const = 0;
			virtual bool canMoveAssign() const = 0;

			virtual String name() const = 0;

			virtual const std::type_info& stdTypeInfo() const = 0;
			virtual std::type_index stdTypeIndex() const = 0;
		};

		template<typename T>
		class TypeImp : public iTypeImp
		{
			virtual bool canDefaultConstruct() const override
			{
				return std::is_default_constructible_v<T>;
			}

			virtual bool canCopy() const override
			{
				return std::is_copy_constructible_v<T>;
			}

			virtual bool canMove() const override
			{
				return std::is_move_constructible_v<T>;
			}

			virtual bool canCopyAssign() const override
			{
				return std::is_copy_assignable_v<T>;
			}

			virtual bool canMoveAssign() const override
			{
				return std::is_move_assignable_v<T>;
			}

			virtual String name() const override
			{
				return StringLiteral(typeid(T).name());
			}

			virtual const std::type_info& stdTypeInfo() const override
			{
				return typeid(T);
			}

			virtual std::type_index stdTypeIndex() const
			{
				return std::type_index(typeid(T));
			}
		};

		InPlace<iTypeImp, AlignedBlockSize_v<TypeImp<int>>, true> mTypeImp;

		Type(bool blank)
		{
		}

	public:

		template<typename T>
		static Type of()
		{
			Type ret(true);
			ret.mTypeImp.setValue<TypeImp<T>>();

			return ret;
		}

		Type()
		{
			mTypeImp.setValue<TypeImp<void>>();
		}

		bool canDefaultConstruct() const
		{
			return mTypeImp->canDefaultConstruct();
		}

		bool canCopy() const
		{
			return mTypeImp->canCopy();
		}

		bool canMove() const
		{
			return mTypeImp->canMove();
		}

		bool canCopyAssign() const
		{
			return mTypeImp->canCopyAssign();
		}

		bool canMoveAssign() const
		{
			return mTypeImp->canMoveAssign();
		}

		String name() const
		{
			return mTypeImp->name();
		}

		const std::type_info& stdTypeInfo() const
		{
			return mTypeImp->stdTypeInfo();
		}

		std::type_index stdTypeIndex() const
		{
			return mTypeImp->stdTypeIndex();
		}

		bool operator<(const Type& right) const
		{
			return (stdTypeIndex() < right.stdTypeIndex());
		}

		bool operator<=(const Type& right) const
		{
			return (stdTypeIndex() <= right.stdTypeIndex());
		}

		bool operator==(const Type& right) const
		{
			return (stdTypeIndex() == right.stdTypeIndex());
		}

		bool operator>=(const Type& right) const
		{
			return (stdTypeIndex() >= right.stdTypeIndex());
		}

		bool operator>(const Type& right) const
		{
			return (stdTypeIndex() > right.stdTypeIndex());
		}
	};

	template<typename T>
	static Type TypeOf()
	{
		return Type::of<T>();
	}
}

#endif // !_STD_EXT_TYPE_H_