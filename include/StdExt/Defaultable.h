#ifndef _STD_EXT_DEFAULTABLE_H_
#define _STD_EXT_DEFAULTABLE_H_

#include "Concepts.h"
#include "Compare.h"

#include <utility>

namespace StdExt
{
	/**
	 * @brief
	 *  Numeric wrapper whose move semantics will return it to its default value when the source of
	 *  of a move operation.  Useful as members of classes where default move operations are desired
	 *  for simplicity and those operations need to return fundamental values to default states.
	 * 
	 * @note
	 *  Warnings that would occur when assigning a value of a different type that T are not
	 *  suppressed in operator implementations.
	 * 
	 * @tparam T
	 *  The type of the value.
	 *
	 * @tparam default_val
	 *  The default value upon construction and to which it will return to when the source of
	 *  a move operation.
	 */
	template<Arithmetic T, T default_val = static_cast<T>(0)>
	class DefaultableMember
	{
		template<Arithmetic other_t, other_t>
		friend class DefaultableMember;

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
		T Value{default_val};
		
		/**
		 * @brief
		 *  Creates an object with a contained value of the _default_val_ template parameter.
		 */
		constexpr DefaultableMember() noexcept = default;

		/**
		 * @brief
		 *  Copy constructor creates object with the same value as the passed object.
		 */
		constexpr DefaultableMember(const DefaultableMember&) noexcept = default;

		/**
		 * @brief
		 *  Move constructor will take the value of _other_, and other will revert back to
		 *  its default value.
		 */
		constexpr DefaultableMember(DefaultableMember&& other) noexcept
			: Value( std::exchange(other.Value, default_val) )
		{
		}
		
		/**
		 * @brief
		 *  Constructs an object containing the converted value of _other_.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr DefaultableMember(const DefaultableMember<other_t, other_def_val>& other) noexcept
			: Value(other.Value)
		{
		}

		/**
		 * @brief
		 *  Move constructor will take the converted value of _other_, and other will revert back to
		 *  its default value.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr DefaultableMember(DefaultableMember<other_t, other_def_val>&& other) noexcept
			: Value( std::exchange(other.Value, other_def_val) )
		{
		}

		/**
		 * @brief
		 *  Sets contained the value of this object to _val_.
		 */
		template<Arithmetic other_t>
		constexpr DefaultableMember(other_t val)
			: Value(val)
		{
		}

		/**
		 * @brief
		 *  Assignment operator copies contained value of right operand into this object.
		 */
		DefaultableMember& operator=(const DefaultableMember&)  noexcept = default;
		
		/**
		 * @brief
		 *  Move operator takes the value of _rhs_ into this container and return _rhs_ to its
		 *  default value.
		 */
		DefaultableMember& operator=(DefaultableMember&& rhs) noexcept
		{
			Value = std::exchange(rhs.Value, default_val);
			return *this;
		}

		/**
		 * @brief
		 *  Assignment operator copies contained value of _rhs_ into this object.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		DefaultableMember& operator=(const DefaultableMember<other_t, other_def_val>& rhs) noexcept
		{
			Value = rhs.Value;
			return *this;
		}

		/**
		 * @brief
		 *  Move operator takes the value of _rhs_ into this container and returns _rhs_ to its
		 *  default value.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		DefaultableMember& operator=(DefaultableMember<other_t, other_def_val>&& rhs) noexcept
		{
			Value = std::exchange(rhs.Value, other_def_val);
			return *this;
		}

		/**
		 * @brief
		 *  Sets the contained value of this object to _rhs_.
		 */
		template<Arithmetic other_t>
		DefaultableMember& operator=(other_t rhs)
		{
			Value = rhs;
			return *this;
		}

		/**
		 * @brief
		 *  Conversion operator for arithmetic types uses the contained value.
		 */
		template<Arithmetic other_t>
		constexpr operator other_t() const
		{
			return static_cast<other_t>(Value);
		}

		/**
		 * @brief
		 *  Returns the sum of the contained value and the contained value of <i>val</i>.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator+(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value + val.Value;
		}

		/**
		 * @brief
		 *  Returns the sum of the contained value and <i>val</i>.
		 */
		template<Arithmetic other_t>
		constexpr auto operator+(other_t val) const
		{
			return Value + val;
		}

		/**
		 * @brief
		 *  Returns the contained value minus the contained value of <i>val</i>.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator-(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value - val.Value;
		}

		/**
		 * @brief
		 *  Returns the contained value minus <i>val</i>.
		 */
		template<Arithmetic other_t>
		constexpr auto operator-(other_t val) const
		{
			return Value - val;
		}

		/**
		 * @brief
		 *  Returns the contained value multiplied by contined value of <i>val</i>.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator*(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value * val.Value;
		}

		/**
		 * @brief
		 *  Returns the contained value multiplied by <i>val</i>.
		 */
		template<Arithmetic other_t>
		constexpr auto operator*(other_t val) const
		{
			return Value * val;
		}

		/**
		 * @brief
		 *  Returns the contained value divided by contined value of <i>val</i>.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator/(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value / val.Value;
		}

		/**
		 * @brief
		 *  Returns the contained value divided by <i>val</i>.
		 */
		template<Arithmetic other_t>
		constexpr auto operator/(other_t val) const
		{
			return Value / val;
		}

		/**
		 * @brief
		 *  Returns the contained value modulus divided by contined value of <i>val</i>.
		 */
		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator%(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value % val.Value;
		}

		/**
		 * @brief
		 *  Returns the contained value modulus divided by <i>val</i>.
		 */
		template<Arithmetic other_t>
		constexpr auto operator%(other_t val) const
		{
			return Value % val;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr bool operator<(const DefaultableMember<other_t, other_def_val>& rhs) const
		{
			return Value < rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr bool operator<(other_t rhs) const
		{
			return Value < rhs;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr bool operator<=(const DefaultableMember<other_t, other_def_val>& rhs) const
		{
			return Value <= rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr bool operator<=(other_t rhs) const
		{
			return Value <= rhs;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr bool operator==(const DefaultableMember<other_t, other_def_val>& rhs) const
		{
			return Value == rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr bool operator==(other_t rhs) const
		{
			return Value == rhs;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr bool operator!=(const DefaultableMember<other_t, other_def_val>& rhs) const
		{
			return Value != rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr bool operator!=(other_t rhs) const
		{
			return Value != rhs;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr bool operator>=(const DefaultableMember<other_t, other_def_val>& rhs) const
		{
			return Value >= rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr bool operator>=(other_t rhs) const
		{
			return Value >= rhs;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr bool operator>(const DefaultableMember<other_t, other_def_val>& rhs) const
		{
			return Value > rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr bool operator>(other_t rhs) const
		{
			return Value > rhs;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator<=>(const DefaultableMember<other_t, other_def_val>& rhs) const
			requires ThreeWayComperableWith<T, other_t>
		{
			return Value <=> rhs.Value;
		}

		template<Arithmetic other_t>
		constexpr auto operator<=>(other_t rhs) const
			requires ThreeWayComperableWith<T, other_t>
		{
			return Value <=> rhs;
		}

		T operator++()
			requires Integral<T>
		{
			return ++Value;
		}

		T operator--()
			requires Integral<T>
		{
			return --Value;
		}

		T operator++(int)
			requires Integral<T>
		{
			return Value++;
		}

		T operator--(int)
			requires Integral<T>
		{
			return Value--;
		}
	};

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr auto operator+(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs + static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr auto operator-(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs - static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr auto operator*(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs * static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr auto operator/(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs / static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr auto operator%(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs % static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr bool operator<(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs < static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr bool operator<=(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs <= static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr bool operator==(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs == static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr bool operator!=(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs != static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr bool operator>=(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs >= static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr bool operator>(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
	{
		return lhs > static_cast<right_t>(rhs);
	}

	template<Arithmetic left_t, Arithmetic right_t, right_t r_def_val>
	constexpr auto operator<=>(left_t lhs, const DefaultableMember<right_t, r_def_val>& rhs)
		requires ThreeWayComperableWith<left_t, right_t>
	{
		return lhs <=> static_cast<right_t>(rhs);
	}
}

#endif // _STD_EXT_DEFAULTABLE_H_