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
	 * @tparam T
	 *  The type of the value.
	 *
	 * @tparam default_val
	 *  The default value upon construction and to which it will return to when the source of
	 *  a move operation.
	 */
	template<Arithmetic T, T default_val = T{}>
	class DefaultableMember
	{
		template<Arithmetic other_t, other_t>
		friend class DefaultableMember;

	public:
		using value_t = T;
		static constexpr T default_value = default_val;

		T Value{default_val};

		constexpr DefaultableMember() = default;
		constexpr DefaultableMember(const DefaultableMember&)  noexcept = default;

		constexpr DefaultableMember(DefaultableMember&& other) noexcept
			: Value( std::exchange(other.Value, default_val) )
		{
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr DefaultableMember(DefaultableMember<other_t, other_def_val>&& other) noexcept
			: Value( std::exchange(other.Value, other_def_val) )
		{
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr DefaultableMember(const DefaultableMember<other_t, other_def_val>& other) noexcept
			: Value( other.Value, other_def_val )
		{
		}

		template<Arithmetic other_t>
		constexpr DefaultableMember(other_t val)
			: Value(val)
		{
		}

		DefaultableMember& operator=(const DefaultableMember&)  noexcept = default;

		DefaultableMember& operator=(DefaultableMember&& rhs) noexcept
		{
			Value = std::exchange(rhs.Value, default_val);
			return *this;
		}

		template<Arithmetic other_t, other_t other_def_val>
		DefaultableMember& operator=(const DefaultableMember<other_t, other_def_val>& rhs) noexcept
		{
			Value = rhs.Value;
			return *this;
		}

		template<Arithmetic other_t, other_t other_def_val>
		DefaultableMember& operator=(DefaultableMember<other_t, other_def_val>&& rhs) noexcept
		{
			Value = std::exchange(rhs.Value, other_def_val);
			return *this;
		}

		template<Arithmetic other_t>
		DefaultableMember& operator=(other_t rhs)
		{
			Value = rhs;
			return *this;
		}

		template<Arithmetic other_t>
		constexpr operator other_t() const
		{
			return static_cast<other_t>(Value);
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator+(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value + val.Value;
		}

		template<Arithmetic other_t>
		constexpr auto operator+(other_t val) const
		{
			return Value + val;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator-(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value - val.Value;
		}

		template<Arithmetic other_t>
		constexpr auto operator-(other_t val) const
		{
			return Value - val;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator*(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value * val.Value;
		}

		template<Arithmetic other_t>
		constexpr auto operator*(other_t val) const
		{
			return Value * val;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator/(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value / val.Value;
		}

		template<Arithmetic other_t>
		constexpr auto operator/(other_t val) const
		{
			return Value / val;
		}

		template<Arithmetic other_t, other_t other_def_val>
		constexpr auto operator%(const DefaultableMember<other_t, other_def_val>& val) const
		{
			return Value % val.Value;
		}

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