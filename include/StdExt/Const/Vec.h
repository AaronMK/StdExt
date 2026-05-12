#ifndef _STD_EXT_CONST_VEC_H_
#define _STD_EXT_CONST_VEC_H_

#include <algorithm>
#include <array>
#include <concepts>
#include <utility>

#include "../Operators.h"

namespace StdExt::Const
{
	/**
	 * @brief
	 *  Compile-time sized collection class that supports element-wise operations
	 *  in constexpr contexts.
	 * 
	 * @details
	 *  This calls does not specifcally optimze for SIMD instructions.  If that optimization
	 *  is desired, use the StdExt::Vec class outside the StdExt::Const namespace.
	 */
	template<DefaultConstructible T, size_t Width>
	class Vec
	{
	private:
		template<DefaultConstructible, size_t>
		friend class Vec;

		std::array<T, Width> mData;

		constexpr Vec(std::array<T, Width> data) noexcept
			: mData(std::move(data))
		{
		}

	public:
		Vec() = default;

		/**
		 * @brief
		 *  Constructs an initialized Vec object.
		 * 
		 * @param args
		 *  Must be either a single argument, or <i>Width</i> arguments.  A single
		 *  argument will initialize each element with that value, <i>Width</i> arguments
		 *  will set each element individually.
		 */
		template<std::convertible_to<T>... args_t>
			requires (sizeof...(args_t) == Width || sizeof...(args_t) == 1)
		constexpr Vec(args_t&&... args) noexcept
		{
			if constexpr (sizeof...(args_t) == 1)
				mData.fill(static_cast<T>((std::forward<args_t>(args), ...)));
			else
				mData = {static_cast<T>(std::forward<args_t>(args))...};
		}

		/**
		 * @brief
		 *  Performs an operation, elementwise, with the value of this %Vec as the left operand
		 *  and <i>rhs</i> as the right operand, and returns a new %Vec of the same size as the result.
		 * 
		 * @tparam U
		 *  The element type of the right operand.
		 * 
		 * @tparam Op
		 *  A functor callable as f(T, U).  The return type will be the element type
		 *  of the result.
		 * 
		 * @param rhs
		 *  Elements applied as the right arguments to the operation.
		 */
		template<typename U, CallableWith<AnyReturn, T, U> auto Op>
		constexpr auto ApplyOp(const Vec<U, Width>& rhs) const noexcept
		{
			using result_t = std::invoke_result_t<decltype(Op), T, U>;
			std::array<result_t, Width> result;
			for (size_t i = 0; i < Width; ++i)
				result[i] = Op(mData[i], rhs.mData[i]);
			return Vec<result_t, Width>(std::move(result));
		}

		/**
		 * @brief
		 *  Elementwise addition. The element type of the result is deduced
		 *  from the addition of T and U.
		 */
		template<typename U>
		constexpr auto operator+(const Vec<U, Width>& rhs) const noexcept
			requires (Plus<T, U>::is_valid)
		{
			return ApplyOp<U, Plus<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise subtraction. The element type of the result is deduced
		 *  from the subtraction of T and U.
		 */
		template<typename U>
		constexpr auto operator-(const Vec<U, Width>& rhs) const noexcept
			requires (Minus<T, U>::is_valid)
		{
			return ApplyOp<U, Minus<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise multiplication. The element type of the result is deduced
		 *  from the multiplication of T and U.
		 */
		template<typename U>
		constexpr auto operator*(const Vec<U, Width>& rhs) const noexcept
			requires (Multiply<T, U>::is_valid)
		{
			return ApplyOp<U, Multiply<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise division. The element type of the result is deduced
		 *  from the division of T and U.
		 */
		template<typename U>
		constexpr auto operator/(const Vec<U, Width>& rhs) const noexcept
			requires (Divide<T, U>::is_valid)
		{
			return ApplyOp<U, Divide<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise modulus. The element type of the result is deduced
		 *  from the modulus of T and U.
		 */
		template<typename U>
		constexpr auto operator%(const Vec<U, Width>& rhs) const noexcept
			requires (Modulus<T, U>::is_valid)
		{
			return ApplyOp<U, Modulus<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise bitwise AND. The element type of the result is deduced
		 *  from the bitwise AND of T and U.
		 */
		template<typename U>
		constexpr auto operator&(const Vec<U, Width>& rhs) const noexcept
			requires (BitwiseAnd<T, U>::is_valid)
		{
			return ApplyOp<U, BitwiseAnd<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise bitwise OR. The element type of the result is deduced
		 *  from the bitwise OR of T and U.
		 */
		template<typename U>
		constexpr auto operator|(const Vec<U, Width>& rhs) const noexcept
			requires (BitwiseOr<T, U>::is_valid)
		{
			return ApplyOp<U, BitwiseOr<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise bitwise XOR. The element type of the result is deduced
		 *  from the bitwise XOR of T and U.
		 */
		template<typename U>
		constexpr auto operator^(const Vec<U, Width>& rhs) const noexcept
			requires (BitwiseXor<T, U>::is_valid)
		{
			return ApplyOp<U, BitwiseXor<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise left shift. The element type of the result is deduced
		 *  from the left shift of T by U.
		 */
		template<typename U>
		constexpr auto operator<<(const Vec<U, Width>& rhs) const noexcept
			requires (ShiftLeft<T, U>::is_valid)
		{
			return ApplyOp<U, ShiftLeft<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise right shift. The element type of the result is deduced
		 *  from the right shift of T by U.
		 */
		template<typename U>
		constexpr auto operator>>(const Vec<U, Width>& rhs) const noexcept
			requires (ShiftRight<T, U>::is_valid)
		{
			return ApplyOp<U, ShiftRight<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise addition assignment.
		 */
		template<typename U>
		constexpr Vec& operator+=(const Vec<U, Width>& rhs) noexcept
			requires (AssignPlus<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] += rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise subtraction assignment.
		 */
		template<typename U>
		constexpr Vec& operator-=(const Vec<U, Width>& rhs) noexcept
			requires (AssignMinus<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] -= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise multiplication assignment.
		 */
		template<typename U>
		constexpr Vec& operator*=(const Vec<U, Width>& rhs) noexcept
			requires (AssignMultiply<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] *= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise division assignment.
		 */
		template<typename U>
		constexpr Vec& operator/=(const Vec<U, Width>& rhs) noexcept
			requires (AssignDivide<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] /= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise modulus assignment.
		 */
		template<typename U>
		constexpr Vec& operator%=(const Vec<U, Width>& rhs) noexcept
			requires (AssignModulus<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] %= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise bitwise AND assignment.
		 */
		template<typename U>
		constexpr Vec& operator&=(const Vec<U, Width>& rhs) noexcept
			requires (AssignBitwiseAnd<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] &= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise bitwise OR assignment.
		 */
		template<typename U>
		constexpr Vec& operator|=(const Vec<U, Width>& rhs) noexcept
			requires (AssignBitwiseOr<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] |= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise bitwise XOR assignment.
		 */
		template<typename U>
		constexpr Vec& operator^=(const Vec<U, Width>& rhs) noexcept
			requires (AssignBitwiseXor<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] ^= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise left shift assignment.
		 */
		template<typename U>
		constexpr Vec& operator<<=(const Vec<U, Width>& rhs) noexcept
			requires (AssignShiftLeft<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] <<= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise right shift assignment.
		 */
		template<typename U>
		constexpr Vec& operator>>=(const Vec<U, Width>& rhs) noexcept
			requires (AssignShiftRight<T, U>::is_valid)
		{
			for (size_t i = 0; i < Width; ++i)
				mData[i] >>= rhs.mData[i];
			return *this;
		}

		/**
		 * @brief
		 *  Elementwise negation.
		 */
		constexpr Vec operator-() const noexcept
			requires (Negate<T>::is_valid)
		{
			std::array<T, Width> result;
			for (size_t i = 0; i < Width; ++i)
				result[i] = -mData[i];
			return Vec(std::move(result));
		}

		/**
		 * @brief
		 *  Elementwise logical NOT, returning a %Vec of bool.
		 */
		constexpr Vec<bool, Width> operator!() const noexcept
			requires (Not<T>::is_valid)
		{
			std::array<bool, Width> result;
			for (size_t i = 0; i < Width; ++i)
				result[i] = !mData[i];
			return Vec<bool, Width>(std::move(result));
		}

		/**
		 * @brief
		 *  Elementwise bitwise complement.
		 */
		constexpr Vec operator~() const noexcept
			requires (BitwiseNot<T>::is_valid)
		{
			std::array<T, Width> result;
			for (size_t i = 0; i < Width; ++i)
				result[i] = ~mData[i];
			return Vec(std::move(result));
		}

		/**
		 * @brief
		 *  Elementwise less-than comparison. The element type of the result is
		 *  deduced from the comparison of T and U.
		 */
		template<typename U>
		constexpr auto operator<(const Vec<U, Width>& rhs) const noexcept
			requires (LessThan<T, U>::is_valid)
		{
			return ApplyOp<U, LessThan<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise less-than-or-equal comparison. The element type of the
		 *  result is deduced from the comparison of T and U.
		 */
		template<typename U>
		constexpr auto operator<=(const Vec<U, Width>& rhs) const noexcept
			requires (LessThanEqual<T, U>::is_valid)
		{
			return ApplyOp<U, LessThanEqual<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise equality comparison. The element type of the result is
		 *  deduced from the comparison of T and U.
		 */
		template<typename U>
		constexpr auto operator==(const Vec<U, Width>& rhs) const noexcept
			requires (Equal<T, U>::is_valid)
		{
			return ApplyOp<U, Equal<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise inequality comparison. The element type of the result is
		 *  deduced from the comparison of T and U.
		 */
		template<typename U>
		constexpr auto operator!=(const Vec<U, Width>& rhs) const noexcept
			requires (NotEqual<T, U>::is_valid)
		{
			return ApplyOp<U, NotEqual<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise greater-than-or-equal comparison. The element type of
		 *  the result is deduced from the comparison of T and U.
		 */
		template<typename U>
		constexpr auto operator>=(const Vec<U, Width>& rhs) const noexcept
			requires (GreaterThanEqual<T, U>::is_valid)
		{
			return ApplyOp<U, GreaterThanEqual<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Elementwise greater-than comparison. The element type of the result
		 *  is deduced from the comparison of T and U.
		 */
		template<typename U>
		constexpr auto operator>(const Vec<U, Width>& rhs) const noexcept
			requires (GreaterThan<T, U>::is_valid)
		{
			return ApplyOp<U, GreaterThan<T, U>{}>(rhs);
		}

		/**
		 * @brief
		 *  Lexicographic three-way comparison. Compares corresponding elements
		 *  in order and returns the ordering result of the first unequal pair,
		 *  or equivalent if all elements compare equal.
		 *
		 * @details
		 *  The comparison category of the result is the common category of
		 *  <code>T <=> U</code> — for example, comparing two @c int vectors
		 *  yields @c std::strong_ordering, while comparing a @c float vector
		 *  with a @c double vector yields @c std::partial_ordering.
		 *
		 * @note
		 *  Unlike the other comparison operators on %Vec, this operator returns
		 *  a single ordering value rather than an elementwise %Vec of results.
		 *  Use <code>operator<</code>, <code>operator<=</code>, etc. for
		 *  elementwise comparisons.
		 */
		template<typename U>
		constexpr auto operator<=>(const Vec<U, Width>& rhs) const noexcept
			requires (ThreeWayCompare<T, U>::is_valid)
		{
			return std::lexicographical_compare_three_way(
				mData.begin(), mData.end(),
				rhs.mData.begin(), rhs.mData.end()
			);
		}

		/**
		 * @brief
		 *  Returns a new %Vec by selecting elements at compile-time indices.
		 *  The width of the result equals the number of indices provided.
		 *
		 * @tparam out_idx
		 *  Indices into this %Vec to select, in the desired output order.
		 *  All indices must be less than Width.
		 */
		template<size_t... out_idx>
		constexpr auto swizzle() const
		{
			static_assert(((out_idx < Width) && ...), "swizzle index out of bounds");
			return Vec<T, sizeof...(out_idx)>{mData[out_idx]...};
		}

		/**
		 * @brief
		 *  Returns a new %Vec by selecting elements at runtime indices. The
		 *  width of the result equals the number of indices provided.
		 *
		 * @param indices
		 *  Indices into this %Vec to select, in the desired output order.
		 */
		template<std::convertible_to<size_t>... idx_t>
		constexpr auto swizzle(idx_t... indices) const
		{
			return Vec<T, sizeof...(idx_t)>{mData[static_cast<size_t>(indices)]...};
		}

		constexpr T& operator[](size_t index)
		{
			assert(index < Width);
			return mData[index];
		}
		
		constexpr const T& operator[](size_t index) const
		{
			assert(index < Width);
			return mData[index];
		}

		constexpr size_t size() const
		{
			return Width;
		}
	};
}

#endif // !_STD_EXT_CONST_VEC_H_
