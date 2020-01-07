#ifndef _STD_EXT_TYPE_H_
#define _STD_EXT_TYPE_H_

#include "Exceptions.h"
#include "InPlace.h"
#include "Utility.h"
#include "String.h"

#include <type_traits>
#include <functional>
#include <memory>

namespace StdExt
{

#pragma region can_invoke
	/**
	 * @brief
	 *  Tests if the callable func_t can be invoked args_t parameters and provide a result that is
	 *  convertable to result_t.
	 */
	template<typename result_t, typename func_t, typename ...args_t>
	struct can_invoke
	{
		template<typename r_t, typename f_t, typename ...a_t>
		static std::is_convertible<std::invoke_result_t<f_t, a_t...>, r_t> test_func(int) {};

		template<typename r_t, typename f_t, typename ...a_t>
		static std::false_type test_func(...) {};

		static constexpr bool value = decltype(test_func<result_t, func_t, args_t...>(0))::value;
	};

	/**
	 * @brief
	 *  Tests if the callable func_t can be invoked args_t parameters and provide a result that is
	 *  convertable to result_t.
	 */
	template<typename result_t, typename func_t, typename ...args_t>
	constexpr bool can_invoke_v = can_invoke<result_t, func_t, args_t...>::value;
#pragma endregion

#pragma region assign
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

		using object_t = std::conditional_t<
			is_pointer,
			std::decay_t<std::remove_pointer_t<T>>,
			std::decay_t<T>
		>;

		using pointer_t = std::add_pointer_t<object_t>;

		static constexpr T default_value()
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