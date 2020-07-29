#ifndef _STD_EXT_TYPE_H_
#define _STD_EXT_TYPE_H_

#include "Exceptions.h"
#include "Concepts.h"
#include "InPlace.h"
#include "Utility.h"
#include "String.h"

#include <type_traits>
#include <functional>
#include <utility>
#include <memory>
#include <tuple>

namespace StdExt
{
	using float32_t = float;
	using float64_t = double;

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

template<int index, typename... types>
using nth_type_t = typename std::tuple_element<index, std::tuple<types...>>::type;

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

	/**
	 * @brief
	 *  Provides information and transformations of types.  This is a combination of grouping
	 *  functionality in the standard library and new type related functionality.  This also
	 *  provides type definitaions that are useful for templated code.
	 */
	template<typename T>
	struct Traits
	{
	private:
		template<typename inner_t, bool more>
		struct Strip;

		template<typename inner_t>
		struct Strip<inner_t, false>
		{
			using type = inner_t;
		};

		template<typename inner_t>
		struct Strip<inner_t, true>
		{
			using stripped = std::remove_cv_t<
				std::remove_pointer_t<
					std::remove_reference_t<inner_t>
				>
			>;

			using type = typename Strip<
				stripped,
				std::is_lvalue_reference_v<stripped> ||
				std::is_rvalue_reference_v<stripped> ||
				std::is_pointer_v<stripped> ||
				std::is_const_v<stripped>
			>::type;
		};

	public:
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
		static constexpr bool is_final = std::is_final_v<T>;
		static constexpr bool is_const = std::is_const_v<T>;
		static constexpr bool is_reference = std::is_lvalue_reference_v<T>;
		static constexpr bool is_move_reference = std::is_rvalue_reference_v<T>;
		static constexpr bool is_class = std::is_class_v<T>;
		static constexpr bool is_enum = std::is_enum_v<T>;
		static constexpr bool is_value = !(is_class || is_reference || is_move_reference);
		static constexpr bool is_core = is_value && !is_pointer;

		static constexpr bool is_const_pointer = (is_pointer && is_const);
		static constexpr bool is_const_reference = (is_reference && is_const);

		/**
		 * @brief
		 *  True if the type does not have any constant, volitile, reference, or pointer specifiers.
		 *
		 * @code
		 * 	StdExt::Traits<int>::is_stripped;         // true
		 * 	StdExt::Traits<int*>::is_stripped;        // false
		 * 	StdExt::Traits<const int>::is_stripped;   // false
		 *
		 * 	StdExt::Traits<std::string>::is_stripped;           // true
		 * 	StdExt::Traits<std::string*>::is_stripped;          // false
		 * 	StdExt::Traits<const std::string>::is_stripped;     // false
		 *
		 * 	StdExt::Traits< std::vector<std::string> >::is_stripped; // true
		 * @endcode
		 */
		static constexpr bool is_stripped = !(is_reference || is_pointer || is_const || is_move_reference);

		/**
		 * @brief
		 *  The type stripped of any constant, volitile, reference, or pointer specifiers.
		 *
		 * @code
		 * 	StdExt::Traits<int>::stripped_t;         // int
		 * 	StdExt::Traits<int*>::stripped_t;        // int
		 * 	StdExt::Traits<const int>::stripped_t;   // int
		 *
		 * 	StdExt::Traits<std::string>::stripped_t;           // std::string
		 * 	StdExt::Traits<std::string*>::stripped_t;          // std::string
		 * 	StdExt::Traits<const std::string>::stripped_t;     // std::string
		 *
		 * 	StdExt::Traits< std::vector<std::string> >::stripped_t;  // std::vector<std::string>
		 * @endcode
		 */
		using stripped_t = typename Strip<T, !is_stripped>::type;

		using pointer_t = typename std::add_pointer_t<stripped_t>;
		using const_pointer_t = typename std::add_const_t<pointer_t>;

		using reference_t = typename std::add_lvalue_reference_t<stripped_t>;
		using const_reference_t = typename std::add_lvalue_reference_t<std::add_const_t<stripped_t>>;

		using move_t = typename std::add_rvalue_reference_t<stripped_t>;

		/**
		 * @brief
		 *  An arugment type that can be used to pass variable of type T as a function
		 *  parameter without a copy step, and without the ability of the function to
		 *  modify the passed variable itself.  Non-constant pointers and references
		 *  can still be used to modify the objects they reference. This basically just
		 *  amounts to structured objects being passed by constant reference.
		 *
		 * @code
		 * 	template<typename T>
		 * 	static void func(typename StdExt::Traits<T>::arg_non_copy_t arg)
		 * 	{
		 * 	}
		 * 	
		 * 	func<std::string>;         // static void func(const std::string&)
		 * 	func<std::string&>;        // static void func(std::string&)
		 * 	func<std::string&&>;       // static void func(std::string&&)
		 * 	func<std::string*>;        // static void func(std::string*)
		 * 	func<const std::string*>;  // static void func(const std::string*)
		 * 	func<int*>;                // static void func(int*)
		 * 	func<int>;                 // static void func(int)
		 * @endcode
		 */
		using arg_non_copy_t = typename std::conditional_t<is_class, const_reference_t, T>;

		/**
		 * @brief
		 *  Provides a type to T that can be trivially be passed into a function without
		 *  calling a copy constructor, and protecting the parameter and what it references from
		 *  modification within the function.  For value types, this will simply be the value
		 *  itself.  For structured types and references, this will be constant reference.
		 *  For pointer types, this will be a constant pointer.
		 *
		 * @code
		 * 	template<typename T>
		 * 	static void func(typename StdExt::Traits<T>::arg_non_copy_t arg)
		 * 	{
		 * 	}
		 * 	
		 * 	func<std::string>;         // static void func(const std::string&)
		 * 	func<std::string&>;        // static void func(const std::string&)
		 * 	func<std::string&&>;       // static void func(const std::string&)
		 * 	func<std::string*>;        // static void func(const std::string*)
		 * 	func<const std::string*>;  // static void func(const std::string*)
		 * 	func<int*>;                // static void func(const int*)
		 * 	func<int>;                 // static void func(int)
		 * @endcode
		 */
		using arg_non_copy_const_t =
			typename std::conditional_t<
				is_class || is_reference || is_move_reference,
				const_reference_t,
				std::conditional_t<
					is_pointer,
					std::add_const_t<T>,
					T
				>
			>;

		/**
		 * brief
		 *  A function that will return a consistant default value for type T.
		 *  For bool this will be false.  For numeric values, it
		 *  will be 0.  For pointers, this will be nullptr.  For classes,
		 *  the default constructor will be used, or an exception will be thrown if
		 *  there is not default constructor.
		 */
		static T default_value()
			requires Defaultable<T>
		{
			if constexpr (std::is_same_v<bool, T>)
			{
				return false;
			}
			else if constexpr (is_arithmetic)
			{
				return T(0);
			}
			else if constexpr(is_pointer)
			{
				return nullptr;
			}
			else if constexpr(default_constructable)
			{
				return T();
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