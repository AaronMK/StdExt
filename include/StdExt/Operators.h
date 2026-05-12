/**
 * @defgroup Operator_Functors Operator Functors
 * 
 * @brief
 *  Functor types implementing operators, with additional functionality to corresponding classes
 *  in the standard library.
 * 
 * @details
 *  These type take the types of the operands on which they will run as template parameters, and
 *  can accept different type for the left and right sides.  
 *
 *  Providing types that are not valid for operator will not generate a compile error, but the
 *  specialized type not contain the call operator.  Its <i>is_valid</i> is a constexpr field
 *  that is true when the types would be valid for the operator.
 */

#ifndef _STD_EXT_OPERATORS_H_
#define _STD_EXT_OPERATORS_H_

#include "Type.h"
#include "Callable.h"

#include <functional>
#include <utility>
#include <type_traits>

namespace StdExt
{
	namespace Detail
	{
		#pragma region Arithmetic

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) plus_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L + R; } )
				return left + right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) minus_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L - R; } )
				return left - right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) multiply_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L * R; } )
				return left * right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) divide_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L / R; } )
				return left / right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) modulus_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L % R; } )
				return left % right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) and_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L && R; } )
				return left && right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) bitwise_and_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L & R; } )
				return left & right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) or_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L || R; } )
				return left || right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) bitwise_or_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L | R; } )
				return left | right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) bitwise_xor_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L ^ R; } )
				return left ^ right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) shift_left_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L << R; } )
				return left << right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) shift_right_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L >> R; } )
				return left >> right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) three_way_compare_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L <=> R; } )
				return left <=> right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) less_than_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L < R; } )
				return left < right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) less_than_equal_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L <= R; } )
				return left <= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) equal_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L == R; } )
				return left == right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) not_equal_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L != R; } )
				return left != right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) greater_than_equal_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L >= R; } )
				return left >= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) greater_than_impl(const left_t& left, const right_t& right)
		{
			if constexpr ( requires (left_t L, right_t R) { L > R; } )
				return left > right;
			else
				return InvalidType();
		}

		#pragma endregion

		#pragma region Assignment

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L = R; } )
				return left = right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_plus_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L += R; } )
				return left += right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_minus_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L -= R; } )
				return left -= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_multiply_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L *= R; } )
				return left *= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_divide_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L /= R; } )
				return left /= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_modulus_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L %= R; } )
				return left %= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_bitwise_and_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L &= R; } )
				return left &= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_bitwise_or_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L |= R; } )
				return left |= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_bitwise_xor_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L ^= R; } )
				return left ^= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_shift_left_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L <<= R; } )
				return left <<= right;
			else
				return InvalidType();
		}

		template<typename left_t, typename right_t>
		static constexpr decltype(auto) assign_shift_right_impl(left_t&& left, const right_t& right)
		{
			if constexpr ( requires (left_t& L, right_t R) { L >>= R; } )
				return left >>= right;
			else
				return InvalidType();
		}

		#pragma endregion

		#pragma region Unary

		template<typename T>
		static constexpr decltype(auto) prefix_increment_impl(T&& val)
		{
			if constexpr ( requires ( T v_arg ) { ++v_arg; } )
				return ++val;
			else
				return InvalidType();
		}

		template<typename T>
		static constexpr decltype(auto) postfix_increment_impl(T&& val)
		{
			if constexpr ( requires (T v_arg ) { v_arg++; } )
				return val++;
			else
				return InvalidType();
		}

		template<typename T>
		static constexpr decltype(auto) prefix_decrement_impl(T&& val)
		{
			if constexpr ( requires (T v_arg ) { --v_arg; } )
				return --val;
			else
				return InvalidType();
		}

		template<typename T>
		static constexpr decltype(auto) postfix_decrement_impl(T&& val)
		{
			if constexpr ( requires (T v_arg ) { v_arg--; } )
				return val--;
			else
				return InvalidType();
		}

		template<typename T>
		static constexpr decltype(auto) negate_impl(const T& val)
		{
			if constexpr ( requires (T v) { -v; } )
				return -val;
			else
				return InvalidType();
		}

		template<typename T>
		static constexpr decltype(auto) not_impl(const T& val)
		{
			if constexpr ( requires (T v) { !v; } )
				return !val;
			else
				return InvalidType();
		}

		template<typename T>
		static constexpr decltype(auto) bitwise_not_impl(const T& val)
		{
			if constexpr ( requires (T v) { ~v; } )
				return ~val;
			else
				return InvalidType();
		}

		#pragma endregion


		template<StaticFunctionPointer auto op_func, typename left_t, typename right_t, bool Mutating = false>
		class BinaryOpTemplate
		{
			using left_param_t = std::conditional_t<Mutating, left_t&&, const left_t&>;
		public:
			using left_type  = left_t;
			using right_type = right_t;

			using result_type = std::invoke_result_t<decltype(op_func), left_t, right_t>;
			static constexpr bool is_valid = !std::same_as<result_type, InvalidType>;

			constexpr decltype(auto) operator()(left_param_t left, const right_t& right) const
				requires (is_valid)
			{
				return std::invoke(op_func, left, right);
			}

			template<typename return_t, typename in_left_t, typename in_right_t>
			constexpr operator CallablePtr<return_t(in_left_t, in_right_t)>() const
				requires (is_valid)
			{
				static_assert( CallableWith<decltype(op_func), return_t, in_left_t, in_right_t> );

				CallablePtr<return_t(in_left_t, in_right_t)> ret;
				ret.template bind<op_func>();

				return ret;
			}
		};

		template<StaticFunctionPointer auto op_func, typename left_t, typename right_t>
		using AssignOpTemplate = BinaryOpTemplate<op_func, left_t, right_t, true>;

		template<StaticFunctionPointer auto op_func, typename T>
		class UnaryOpTemplate
		{
		public:
			using result_type = std::invoke_result_t<decltype(op_func), T>;
			static constexpr bool is_valid = !std::same_as<result_type, InvalidType>;

			constexpr decltype(auto) operator()(T&& val) const
				requires (is_valid)
			{
				return std::invoke(op_func, val);
			}

			template<typename return_t, typename in_t>
			constexpr operator CallablePtr<return_t(in_t)>() const
				requires (is_valid)
			{
				static_assert( CallableWith<decltype(op_func), return_t, in_t> );

				CallablePtr<return_t(in_t)> ret;
				ret.template bind<op_func>();

				return ret;
			}
		};
	}

	#pragma region Arithmetic

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Plus = Detail::BinaryOpTemplate<&Detail::plus_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Minus = Detail::BinaryOpTemplate<&Detail::minus_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Multiply = Detail::BinaryOpTemplate<&Detail::multiply_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Divide = Detail::BinaryOpTemplate<&Detail::divide_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Modulus = Detail::BinaryOpTemplate<&Detail::modulus_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using And = Detail::BinaryOpTemplate<&Detail::and_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using BitwiseAnd = Detail::BinaryOpTemplate<&Detail::bitwise_and_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Or = Detail::BinaryOpTemplate<&Detail::or_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using BitwiseOr = Detail::BinaryOpTemplate<&Detail::bitwise_or_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using BitwiseXor = Detail::BinaryOpTemplate<&Detail::bitwise_xor_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using ShiftLeft = Detail::BinaryOpTemplate<&Detail::shift_left_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using ShiftRight = Detail::BinaryOpTemplate<&Detail::shift_right_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using ThreeWayCompare = Detail::BinaryOpTemplate<&Detail::three_way_compare_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using LessThan = Detail::BinaryOpTemplate<&Detail::less_than_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using LessThanEqual = Detail::BinaryOpTemplate<&Detail::less_than_equal_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Equal = Detail::BinaryOpTemplate<&Detail::equal_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using NotEqual = Detail::BinaryOpTemplate<&Detail::not_equal_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using GreaterThanEqual = Detail::BinaryOpTemplate<&Detail::greater_than_equal_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using GreaterThan = Detail::BinaryOpTemplate<&Detail::greater_than_impl<left_t, right_t>, left_t, right_t>;

	#pragma endregion

	#pragma region Assignment

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using Assign = Detail::AssignOpTemplate<&Detail::assign_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignPlus = Detail::AssignOpTemplate<&Detail::assign_plus_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignMinus = Detail::AssignOpTemplate<&Detail::assign_minus_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignMultiply = Detail::AssignOpTemplate<&Detail::assign_multiply_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignDivide = Detail::AssignOpTemplate<&Detail::assign_divide_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignModulus = Detail::AssignOpTemplate<&Detail::assign_modulus_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignBitwiseAnd = Detail::AssignOpTemplate<&Detail::assign_bitwise_and_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignBitwiseOr = Detail::AssignOpTemplate<&Detail::assign_bitwise_or_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignBitwiseXor = Detail::AssignOpTemplate<&Detail::assign_bitwise_xor_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignShiftLeft = Detail::AssignOpTemplate<&Detail::assign_shift_left_impl<left_t, right_t>, left_t, right_t>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename left_t, typename right_t = left_t>
	using AssignShiftRight = Detail::AssignOpTemplate<&Detail::assign_shift_right_impl<left_t, right_t>, left_t, right_t>;

	#pragma endregion

	#pragma region Unary

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using PrefixIncrement = Detail::UnaryOpTemplate<&Detail::prefix_increment_impl<T>, T>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using PostfixIncrement = Detail::UnaryOpTemplate<&Detail::postfix_increment_impl<T>, T>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using PrefixDecrement = Detail::UnaryOpTemplate<&Detail::prefix_decrement_impl<T>, T>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using PostfixDecrement = Detail::UnaryOpTemplate<&Detail::postfix_decrement_impl<T>, T>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using Negate = Detail::UnaryOpTemplate<&Detail::negate_impl<T>, T>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using Not = Detail::UnaryOpTemplate<&Detail::not_impl<T>, T>;

	/**
	 * @ingroup Operator_Functors
	 */
	template<typename T>
	using BitwiseNot = Detail::UnaryOpTemplate<&Detail::bitwise_not_impl<T>, T>;

	#pragma endregion
}

#endif // _STD_EXT_OPERATORS_H_