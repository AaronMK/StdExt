#ifndef _STD_EXT_TYPE_H_
#define _STD_EXT_TYPE_H_

#include "Exceptions.h"
#include "InPlace.h"
#include "Utility.h"
#include "String.h"

#include <type_traits>
#include <functional>

namespace StdExt
{
	template<typename T = void>
	struct assign
	{
		constexpr void operator()(T left, T right)
		{
			left = right;
		}
	};

	template<>
	struct assign<void>
	{
		template<typename left_t, typename right_t>
		constexpr void operator()(left_t left, const right_t right)
		{
			left = right;
		}
	};

#pragma region can_assign_from
	template<typename target_t, typename... _Types>
	struct can_assign_from;

	template<typename target_t, typename value_t>
	struct can_assign_from<target_t, value_t>
	{
		static constexpr bool value = std::is_convertible_v<value_t, target_t>;
	};

	template<typename target_t, typename a_t, typename b_t, typename... rest_t>
	struct can_assign_from<target_t, a_t, b_t, rest_t...>
	{
		static constexpr bool value =
			can_assign_from<target_t, a_t>::value &&
			can_assign_from<target_t, b_t, rest_t...>::value;
	};

	template<typename target_t, typename a_t, typename... rest_t>
	constexpr bool can_assign_from_v = can_assign_from<target_t, a_t, rest_t...>::value;
#pragma endregion

	template<typename T>
	struct Traits
	{
		static constexpr bool has_equality = can_invoke_v<bool, std::equal_to<>, T, T>;
		static constexpr bool has_inequality = can_invoke_v<bool, std::not_equal_to<>, T, T>;
		static constexpr bool has_less_than = can_invoke_v<bool, std::less<>, T, T>;
		static constexpr bool has_less_equal = can_invoke_v<bool, std::less_equal<>, T, T>;
		static constexpr bool has_greater_than = can_invoke_v<bool, std::greater<>, T, T>;
		static constexpr bool has_greater_equal = can_invoke_v<bool, std::greater_equal<>, T, T>;

		static constexpr bool default_constructable = std::is_default_constructible_v<T>;
		static constexpr bool copy_constructable = std::is_copy_constructible_v<T>;
		static constexpr bool move_constructable = std::is_move_constructible_v<T>;
		static constexpr bool copy_assignable = std::is_copy_assignable_v<T>;
		static constexpr bool move_assignable = std::is_move_assignable_v<T>;

		static constexpr bool is_void = std::is_void_v<T>;
		static constexpr bool is_arithmetic = std::is_arithmetic_v<T>;
		static constexpr bool is_signed = std::is_signed_v<T>;
		static constexpr bool is_unsigned = std::is_unsigned_v<T>;
		static constexpr bool is_pointer = std::is_pointer_v<T>;
		static constexpr bool is_const = std::is_const_v<T>;
		static constexpr bool is_reference = std::is_lvalue_reference_v<T>;
		static constexpr bool is_move_reference = std::is_rvalue_reference_v<T>;

		static constexpr bool is_class = std::is_class_v<T>;

		static T default_value()
		{
			if constexpr (std::is_same_v<bool, T>)
			{
				return false;
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				return T(0);
			}
			else if constexpr(std::is_default_constructible_v<T>)
			{
				return T();
			}
			else
			{
				throw invalid_operation(
					"Attempting to create a default value of a type without a default constructor."
				);
			}
		}
	};

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

			virtual bool isVoid() const = 0;
			virtual bool isConstant() const = 0;
			virtual bool isPointer() const = 0;
			virtual bool isReference() const = 0;
			virtual bool isMove() const = 0;

			virtual bool isClass() const = 0;

			virtual String name() const = 0;

			virtual const std::type_info& stdTypeInfo() const = 0;
			virtual std::type_index stdTypeIndex() const = 0;
		};

		template<typename T>
		class TypeImp : public iTypeImp
		{
			virtual bool canDefaultConstruct() const override
			{
				return Traits<T>::default_constructable;
			}

			virtual bool canCopy() const override
			{
				return Traits<T>::copy_constructable;
			}

			virtual bool canMove() const override
			{
				return Traits<T>::move_constructable;
			}

			virtual bool canCopyAssign() const override
			{
				return Traits<T>::copy_assignable;
			}

			virtual bool canMoveAssign() const override
			{
				return Traits<T>::move_assignable;
			}

			virtual bool isVoid() const  override
			{
				return Traits<T>::is_void;
			}

			virtual bool isConstant() const  override
			{
				return Traits<T>::is_const;
			}

			virtual bool isPointer() const  override
			{
				return Traits<T>::is_pointer;
			}

			virtual bool isReference() const  override
			{
				return Traits<T>::is_reference;
			}

			virtual bool isMove() const  override
			{
				return Traits<T>::is_move_reference;
			}


			virtual bool isClass() const  override
			{
				return Traits<T>::is_class;
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

		bool isVoid() const
		{
			return mTypeImp->isVoid();
		}

		bool isConstant() const
		{
			return mTypeImp->isConstant();
		}

		bool isPointer() const
		{
			return mTypeImp->isPointer();
		}

		bool isReference() const
		{
			return mTypeImp->isReference();
		}

		bool isMove() const
		{
			return mTypeImp->isMove();
		}

		bool isClass() const
		{
			return mTypeImp->isClass();
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