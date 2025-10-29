#ifndef _STD_EXT_DEFAULTABLE_H_
#define _STD_EXT_DEFAULTABLE_H_

#include "Concepts.h"
#include "Compare.h"
#include "Operators.h"

#include <utility>

namespace StdExt
{
	/**
	 * @brief
	 *  Wrapper whose move semantics will return it to its default value when the source of
	 *  of a move operation.  Useful as members of classes where default move operations are desired
	 *  for simplicity and those operations need to return values to default states.
	 * 
	 * @note
	 *  Warnings that would occur when assigning a value of a different type than T are not
	 *  suppressed in operator implementations.
	 * 
	 * @tparam T
	 *  The type of the value.
	 *
	 * @tparam default_val
	 *  The default value upon construction and to which it will return to when the source of
	 *  a move operation.
	 */
	template<typename T, T default_val = T{}>
	class Defaultable
	{
		static_assert(CopyConstructable<T> && CopyAssignable<T>, "T must support copy semantics.");
		static_assert(MoveConstructable<T> && MoveAssignable<T>, "T must support move semantics.");

		template<typename other_t, other_t other_def_val>
		friend class Defaultable;

	public:
		using value_t = T;

		/**
		 * @brief
		 *  The default value of the object upon construction and after being the source
		 *  of a move operation.
		 */
		static constexpr T default_value = default_val;

		/**
		 * @brief
		 *  The value of the object.
		 */
		T Value = default_value;
		
		/**
		 * @brief
		 *  Creates an object with a contained value of the _default_val_ template parameter.
		 */
		constexpr Defaultable() noexcept = default;

		/**
		 * @brief
		 *  Copy constructor creates object with the same value as the passed object.
		 */
		constexpr Defaultable(const Defaultable&) noexcept = default;

		/**
		 * @brief
		 *  Move constructor will take the value of _other_, and other will revert back to
		 *  its default value.
		 */
		constexpr Defaultable(Defaultable&& other) noexcept
			: Value( std::exchange(other.Value, default_value) )
		{
		}
		
		/**
		 * @brief
		 *  Constructs an object containing the converted value of _other_.
		 */
		template<typename other_t, other_t other_def_val>
			requires std::constructible_from<T, other_t>
		constexpr Defaultable(const Defaultable<other_t, other_def_val>& other) noexcept
			: Value(other.Value)
		{
		}

		/**
		 * @brief
		 *  Move constructor will take the converted value of _other_, and other will revert back to
		 *  its default value.
		 */
		template<typename other_t, other_t other_def_val>
			requires std::constructible_from<T, other_t>
		constexpr Defaultable(Defaultable<other_t, other_def_val>&& other) noexcept
			: Value( std::exchange(other.Value, other_def_val) )
		{
		}

		/**
		 * @brief
		 *  Sets contained the value of this object to _val_.
		 */
		template<typename other_t>
			requires std::constructible_from<T, other_t>
		constexpr Defaultable(other_t val)
			: Value(val)
		{
		}

		/**
		 * @brief
		 *  Assignment operator copies contained value of right operand into this object.
		 */
		Defaultable& operator=(const Defaultable&)  noexcept = default;
		
		/**
		 * @brief
		 *  Move operator takes the value of _rhs_ into this container and return _rhs_ to its
		 *  default value.
		 */
		Defaultable& operator=(Defaultable&& rhs) noexcept
		{
			Value = std::exchange(rhs.Value, default_value);
			return *this;
		}

		/**
		 * @brief
		 *  Assignment operator copies contained value of _rhs_ into this object.
		 */
		template<AssignableTo<T> other_t, other_t other_def_val>
		Defaultable& operator=(const Defaultable<other_t, other_def_val>& rhs) noexcept
		{
			Value = rhs.Value;
			return *this;
		}

		/**
		 * @brief
		 *  Move operator takes the value of _rhs_ into this container and returns _rhs_ to its
		 *  default value.
		 */
		template<AssignableTo<T> other_t, other_t other_def_val>
		Defaultable& operator=(Defaultable<other_t, other_def_val>&& rhs) noexcept
		{
			Value = std::exchange(rhs.Value, other_def_val);
			return *this;
		}

		/**
		 * @brief
		 *  Sets the contained value of this object to _rhs_.
		 */
		template<AssignableTo<T> other_t>
		Defaultable& operator=(other_t rhs)
		{
			Value = rhs;
			return *this;
		}

		/**
		 * @brief
		 *  Conversion operator for arithmetic types uses the contained value.
		 */
		template<ImplicitlyConvertableTo<T> other_t>
		constexpr operator other_t() const
		{
			return static_cast<other_t>(Value);
		}

		decltype(auto) operator++()
			requires PrefixIncrement<T>::is_valid
		{
			return ++Value;
		}

		decltype(auto) operator--()
			requires PrefixDecrement<T>::is_valid
		{
			return --Value;
		}

		decltype(auto) operator++(int)
			requires PostfixIncrement<T>::is_valid
		{
			return Value++;
		}

		decltype(auto) operator--(int)
			requires PostfixDecrement<T>::is_valid
		{
			return Value--;
		}

		constexpr decltype(auto) operator[](size_t index) const
			requires PointerType<T>
		{
			return Value[index];
		}

		constexpr decltype(auto) operator[](size_t index)
			requires PointerType<T>
		{
			return Value[index];
		}

		constexpr decltype(auto) operator->()
			requires PointerType<T>
		{
			return Value;
		}

		constexpr decltype(auto) operator->() const
			requires PointerType<T>
		{
			return Value;
		}
	};

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator+(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Plus<left_t, right_t>::is_valid
	{
		return lhs.Value + rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator+(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires Plus<left_t, right_t>::is_valid
	{
		return lhs.Value + rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator+(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Plus<left_t, right_t>::is_valid
	{
		return lhs + rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator+=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignPlus<left_t, right_t>::is_valid
	{
		return lhs.Value += rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator+=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignPlus<left_t, right_t>::is_valid
	{
		return lhs.Value += rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator+=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignPlus<left_t, right_t>::is_valid
	{
		return lhs += rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator-(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Minus<left_t, right_t>::is_valid
	{
		return lhs.Value - rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator-(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires Minus<left_t, right_t>::is_valid
	{
		return lhs.Value - rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator-(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Minus<left_t, right_t>::is_valid
	{
		return lhs - rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator-=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignMinus<left_t, right_t>::is_valid
	{
		return lhs.Value -= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator-=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignMinus<left_t, right_t>::is_valid
	{
		return lhs.Value -= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator-=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignMinus<left_t, right_t>::is_valid
	{
		return lhs -= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator*(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Multiply<left_t, right_t>::is_valid
	{
		return lhs.Value * rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator*(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires Multiply<left_t, right_t>::is_valid
	{
		return lhs.Value * rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator*(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Multiply<left_t, right_t>::is_valid
	{
		return lhs * rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator*=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignMultiply<left_t, right_t>::is_valid
	{
		return lhs.Value *= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator*=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignMultiply<left_t, right_t>::is_valid
	{
		return lhs.Value *= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator*=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignMultiply<left_t, right_t>::is_valid
	{
		return lhs *= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator/(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Divide<left_t, right_t>::is_valid
	{
		return lhs.Value / rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator/(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires Divide<left_t, right_t>::is_valid
	{
		return lhs.Value / rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator/(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Divide<left_t, right_t>::is_valid
	{
		return lhs / rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator/=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignDivide<left_t, right_t>::is_valid
	{
		return lhs.Value /= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator/=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignDivide<left_t, right_t>::is_valid
	{
		return lhs.Value /= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator/=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignDivide<left_t, right_t>::is_valid
	{
		return lhs /= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator%(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Modulus<left_t, right_t>::is_valid
	{
		return lhs.Value % rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator%(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires Modulus<left_t, right_t>::is_valid
	{
		return lhs.Value % rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator%(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Modulus<left_t, right_t>::is_valid
	{
		return lhs % rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator%=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignModulus<left_t, right_t>::is_valid
	{
		return lhs.Value %= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator%=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignModulus<left_t, right_t>::is_valid
	{
		return lhs.Value %= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator%=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignModulus<left_t, right_t>::is_valid
	{
		return lhs %= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator&(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires BitwiseAnd<left_t, right_t>::is_valid
	{
		return lhs.Value & rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator&(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires BitwiseAnd<left_t, right_t>::is_valid
	{
		return lhs.Value & rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator&(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires BitwiseAnd<left_t, right_t>::is_valid
	{
		return lhs & rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator&=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignBitwiseAnd<left_t, right_t>::is_valid
	{
		return lhs.Value &= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator&=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignBitwiseAnd<left_t, right_t>::is_valid
	{
		return lhs.Value &= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator&=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignBitwiseAnd<left_t, right_t>::is_valid
	{
		return lhs &= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator|(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires BitwiseOr<left_t, right_t>::is_valid
	{
		return lhs.Value | rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator|(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires BitwiseOr<left_t, right_t>::is_valid
	{
		return lhs.Value | rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator|(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires BitwiseOr<left_t, right_t>::is_valid
	{
		return lhs | rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator|=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignBitwiseOr<left_t, right_t>::is_valid
	{
		return lhs.Value |= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator|=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignBitwiseOr<left_t, right_t>::is_valid
	{
		return lhs.Value |= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator|=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignBitwiseOr<left_t, right_t>::is_valid
	{
		return lhs |= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator^(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires BitwiseXor<left_t, right_t>::is_valid
	{
		return lhs.Value ^ rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator^(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires BitwiseXor<left_t, right_t>::is_valid
	{
		return lhs.Value ^ rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator^(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires BitwiseXor<left_t, right_t>::is_valid
	{
		return lhs ^ rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator^=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignBitwiseXor<left_t, right_t>::is_valid
	{
		return lhs.Value ^= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator^=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignBitwiseXor<left_t, right_t>::is_valid
	{
		return lhs.Value ^= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator^=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignBitwiseXor<left_t, right_t>::is_valid
	{
		return lhs ^= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator<<(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires ShiftLeft<left_t, right_t>::is_valid
	{
		return lhs.Value << rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator<<(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires ShiftLeft<left_t, right_t>::is_valid
	{
		return lhs.Value << rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator<<(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires ShiftLeft<left_t, right_t>::is_valid
	{
		return lhs << rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator<<=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignShiftLeft<left_t, right_t>::is_valid
	{
		return lhs.Value <<= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator<<=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignShiftLeft<left_t, right_t>::is_valid
	{
		return lhs.Value <<= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator<<=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignShiftLeft<left_t, right_t>::is_valid
	{
		return lhs <<= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator>>(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires ShiftRight<left_t, right_t>::is_valid
	{
		return lhs.Value >> rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator>>(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires ShiftRight<left_t, right_t>::is_valid
	{
		return lhs.Value >> rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator>>(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires ShiftRight<left_t, right_t>::is_valid
	{
		return lhs >> rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator>>=(Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignShiftRight<left_t, right_t>::is_valid
	{
		return lhs.Value >>= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr decltype(auto) operator>>=(Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires AssignShiftRight<left_t, right_t>::is_valid
	{
		return lhs.Value >>= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr decltype(auto) operator>>=(left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires AssignShiftRight<left_t, right_t>::is_valid
	{
		return lhs >>= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator<(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires LessThan<left_t, right_t>::is_valid
	{
		return lhs.Value < rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator<(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires LessThan<left_t, right_t>::is_valid
	{
		return lhs.Value < rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator<(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires LessThan<left_t, right_t>::is_valid
	{
		return lhs < rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator<=(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires LessThanEqual<left_t, right_t>::is_valid
	{
		return lhs.Value <= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator<=(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires LessThanEqual<left_t, right_t>::is_valid
	{
		return lhs.Value <= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator<=(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires LessThanEqual<left_t, right_t>::is_valid
	{
		return lhs <= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator==(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Equal<left_t, right_t>::is_valid
	{
		return lhs.Value == rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator==(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires Equal<left_t, right_t>::is_valid
	{
		return lhs.Value == rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator==(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires Equal<left_t, right_t>::is_valid
	{
		return lhs == rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator!=(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires NotEqual<left_t, right_t>::is_valid
	{
		return lhs.Value != rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator!=(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires NotEqual<left_t, right_t>::is_valid
	{
		return lhs.Value != rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator!=(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires NotEqual<left_t, right_t>::is_valid
	{
		return lhs != rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator>=(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires GreaterThanEqual<left_t, right_t>::is_valid
	{
		return lhs.Value >= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator>=(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires GreaterThanEqual<left_t, right_t>::is_valid
	{
		return lhs.Value >= rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator>=(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires GreaterThanEqual<left_t, right_t>::is_valid
	{
		return lhs >= rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator>(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires GreaterThan<left_t, right_t>::is_valid
	{
		return lhs.Value > rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator>(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires GreaterThan<left_t, right_t>::is_valid
	{
		return lhs.Value > rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator>(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires GreaterThan<left_t, right_t>::is_valid
	{
		return lhs > rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t, right_t r_def_val>
	constexpr auto operator<=>(const Defaultable<left_t, l_def_val>& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires ThreeWayCompare<left_t, right_t>::is_valid
	{
		return lhs.Value <=> rhs.Value;
	}

	template<typename left_t, left_t l_def_val, typename right_t>
	constexpr auto operator<=>(const Defaultable<left_t, l_def_val>& lhs, const right_t& rhs)
		requires ThreeWayCompare<left_t, right_t>::is_valid
	{
		return lhs.Value <=> rhs;
	}

	template<typename left_t, typename right_t, right_t r_def_val>
	constexpr auto operator<=>(const left_t& lhs, const Defaultable<right_t, r_def_val>& rhs)
		requires ThreeWayCompare<left_t, right_t>::is_valid
	{
		return lhs <=> rhs.Value;
	}
}

#endif // _STD_EXT_DEFAULTABLE_H_