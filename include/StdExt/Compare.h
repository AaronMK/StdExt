/**
 * @file
 *  The comparison concepts in StdExt have more stringent requirements than those in
 *  the standard library.  They must return bool, not just a type convertible to bool.
 *  It also defines a HasCompare concept, and compare functions that will prioritize
 *  a compare function for classes that implement it.
 */

#ifndef _STD_EXT_COMPARE_H_
#define _STD_EXT_COMPARE_H_

#include "Concepts.h"

#include <compare>

namespace StdExt
{
	namespace Detail
	{
		template<typename... comp_types>
		struct OrderingResultImpl;

		template<typename left_t, typename right_t>
		struct OrderingResultImpl<left_t, right_t>
		{
			using result_t = std::compare_three_way_result_t<left_t, right_t>;
		};

		template<typename left_1_t, typename right_1_t, typename left_2_t, typename right_2_t>
		struct OrderingResultImpl<left_1_t, right_1_t, left_2_t, right_2_t>
		{
			using result_t = std::common_comparison_category_t<
				typename OrderingResultImpl<left_1_t, right_1_t>::result_t,
				typename OrderingResultImpl<left_2_t, right_2_t>::result_t
			>;
		};

		template<typename left_t, typename right_t, typename... remaining_t>
		struct OrderingResultImpl<left_t, right_t, remaining_t...>
		{
			using result_t = std::common_comparison_category_t<
				typename OrderingResultImpl<left_t, right_t>::result_t,
				typename OrderingResultImpl<remaining_t...>::result_t
			>;
		};
	}

	template<typename... T>
	using OrderingResult = Detail::OrderingResultImpl<T...>::result_t;

	/**
	 * @brief
	 *  The type is one of the three standard ordering classes.
	 */
	template<typename T>
	concept OrderingClass = AnyOf<T, std::weak_ordering, std::partial_ordering, std::strong_ordering>;
	
	/**
	 * @brief
	 *  The type has a less than operator with with_t that returns bool.
	 */
	template<typename T, typename with_t>
	concept HasLessThanWith = requires (T L, with_t R)
	{
		{ L < R } -> std::same_as<bool>;
	};

	/**
	 * @brief
	 *  The type has a less than or equal operator with with_t that returns bool.
	 */
	template<typename T, typename with_t>
	concept HasLessThanEqualWith = requires (T L, with_t R)
	{
		{ L <= R } -> std::same_as<bool>;
	};

	/**
	 * @brief
	 *  The type has an equality operator with with_t that returns bool.
	 */
	template<typename T, typename with_t>
	concept HasEqualsWith = requires (T L, with_t R)
	{
		{ L == R } -> std::same_as<bool>;
	};

	/**
	 * @brief
	 *  The type has an inequality operator with with_t that returns bool.
	 */
	template<typename T, typename with_t>
	concept HasNotEqualWith = requires (T L, with_t R)
	{
		{ L != R } -> std::same_as<bool>;
	};

	/**
	 * @brief
	 *  The type has a greater than or equal operator with with_t that returns bool.
	 */
	template<typename T, typename with_t>
	concept HasGreaterThanEqualWith = requires (T L, with_t R)
	{
		{ L >= R } -> std::same_as<bool>;
	};

	/**
	 * @brief
	 *  The type has a greater than operator with with_t that returns bool.
	 */
	template<typename T, typename with_t>
	concept HasGreaterThanWith = requires (T L, with_t R)
	{
		{ L > R } -> std::same_as<bool>;
	};

	/**
	 * @brief
	 *  The type has a compare() member function that takes a with_t argument
	 *  and returns an int result.
	 */
	template<typename T, typename with_t>
	concept HasCompareWith = requires (T Val, const with_t& other)
	{
		{ Val.compare(other) } -> std::same_as<int>;
	};

	/**
	 * @brief
	 *  The type has a three-way comparison operator with with_t.  It can be of
	 *  any of the three standard ordering categories.
	 */
	template<typename T, typename with_t>
	concept ThreeWayComperableWith = 
		std::three_way_comparable_with<T, with_t, std::strong_ordering> ||
		std::three_way_comparable_with<T, with_t, std::partial_ordering> ||
		std::three_way_comparable_with<T, with_t, std::weak_ordering>;

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::equals()
	 *  to evaluate equality with a value of type with_t.
	 */
	template<typename T, typename with_t>
	concept EqualityComperableWith = 
		HasEqualsWith<T, with_t> ||
		ThreeWayComperableWith<T, with_t> ||
		HasCompareWith<T, with_t>;

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::compare()
	 *  to compare with a value of type with_t.
	 */
	template<typename T, typename with_t>
	concept ComperableWith = 
		(HasLessThanWith<T, with_t> && HasGreaterThanWith<T, with_t>) ||
		ThreeWayComperableWith<T, with_t> || HasCompareWith<T, with_t>;
	
	/**
	 * @brief
	 *  The type has a less than operator that returns bool.
	 */
	template<typename T>
	concept HasLessThan = HasLessThanWith<T, T>;

	/**
	 * @brief
	 *  The type has a less than or equal operator that returns bool.
	 */
	template<typename T>
	concept HasLessThanEqual = HasLessThanEqualWith<T, T>;

	/**
	 * @brief
	 *  The type has an equality operator that returns bool.
	 */
	template<typename T>
	concept HasEquals = HasEqualsWith<T, T>;

	/**
	 * @brief
	 *  The type has an inequality operator that returns bool.
	 */
	template<typename T>
	concept HasNotEqual = HasNotEqualWith<T, T>;

	/**
	 * @brief
	 *  The type has a greater than or equal operator that returns bool.
	 */
	template<typename T>
	concept HasGreaterThanEqual = HasGreaterThanEqualWith<T, T>;

	/**
	 * @brief
	 *  The type has a greater than operator that returns bool.
	 */
	template<typename T>
	concept HasGreaterThan = HasGreaterThanWith<T, T>;

	/**
	 * @brief
	 *  The type has a compare() member function that takes an argument of the same
	 *  type and returns an int result.
	 */
	template<typename T>
	concept HasCompare = HasCompareWith<T, T>;

	/**
	 * @brief
	 *  The type has a three-way comparison operator.  It can be of
	 *  any of the three standard ordering catagories.
	 */
	template<typename T>
	concept ThreeWayComperable = 
		std::three_way_comparable<T, std::strong_ordering> ||
		std::three_way_comparable<T, std::partial_ordering> ||
		std::three_way_comparable<T, std::weak_ordering>;

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::equals()
	 *  to evaluate equality.
	 */
	template<typename T>
	concept EqualityComperable =
		HasEquals<T> ||
		ThreeWayComperable<T> ||
		HasCompare<T>;

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::compare()
	 *  to compare with a value of same type.
	 */
	template<typename T>
	concept Comperable = 
		HasCompare<T> || ThreeWayComperable<T> ||
		(HasLessThan<T> && HasGreaterThan<T>);

	/**
	 * @brief
	 *  Takes an ordering class value and gets a standard -1, 0, 1 integer compare
	 *  result from it.
	 */
	template<OrderingClass T>
	static constexpr int orderingToInt(T cmp)
	{
		return (cmp < 0) ? -1 : ( (cmp > 0) ? 1 : 0 ); 
	}

	template<Arithmetic T>
	constexpr double relativeDifference(T left, T right)
	{
		if (left == right)
			return 0.0;

		const double d_left = static_cast<double>(left);
		const double d_right = static_cast<double>(right);

		return std::abs((d_left - d_right) / std::min(d_left, d_right));
	}

	/**
	 * @brief
	 *  Tests for approximate equality of left and right.  For floating point types,
	 *  this will pass if the relative error is within the threshold parameter.
	 *  For integral types, this compiles down to a standard equality test.
	 */
	template<Arithmetic T>
	constexpr bool approxEqual(T left, T right, float threshold = 0.0001)
	{
		if constexpr (Integral<T>)
		{
			return (left == right);
		}
		else
		{
			constexpr T zero = T(0.0);

			if (left == right)
				return true;

			if (left == zero || right == zero)
				return false;

			if (std::isnan(left) && std::isnan(right))
				return false;

			T relative_error = fabs((left - right) / std::min(left, right));
			return (relative_error <= T(threshold));
		}
	}

	/**
	 * @brief
	 *  Runs approximate compare logic for arithmetic types.
	 */
	template<Arithmetic T>
	constexpr int approxCompare(T left, T right)
	{
		if (approxEqual(left, right))
			return 0;

		if (left < right)
			return -1;
		
		return 1;
	}

	/**
	 * @brief
	 *  Runs approximate compare logic iteratively on each set of two parameters until it
	 *  finds a pair that are not equal and returns the result of that compare operation.
	 *  If all pairs are equal, zero is returned.
	 */
	template<Arithmetic t_a, std::same_as<t_a> t_b, Arithmetic ...args_t>
	constexpr int approxCompare(t_a left, const t_b right, args_t ...args)
	{
		static_assert(sizeof...(args) % 2 == 0);

		int comp_result = approxCompare<t_a>(left, right);

		if (comp_result != 0)
		{
			return comp_result;
		}
		else
		{
			if constexpr ( sizeof...(args) > 0 )
				return approxCompare(args...);
			else
				return 0;
		}
	}

	/**
	 * @brief
	 *  Runs a comparison operation on iteratively on each set of two parameters until it
	 *  finds a pair that are not equal and returns the result of that compare operation.
	 */
	template<ThreeWayComperable left_t, ThreeWayComperableWith<left_t> right_t, typename ...args_t>
	constexpr auto compare(const left_t& left, const right_t& right, const args_t& ...args) noexcept
	{	
		using return_t = OrderingResult<left_t, right_t, args_t...>;

		static_assert(sizeof...(args) % 2 == 0);
		static_assert( OrderingClass<return_t> );

		if constexpr ( 0 == sizeof...(args) )
		{
			return static_cast<return_t>(left <=> right);
		}
		else
		{
			const auto comp_result = static_cast<return_t>(left <=> right);

			if constexpr ( std::same_as<return_t, std::partial_ordering> )
			{
				return ( std::partial_ordering::unordered == comp_result || std::partial_ordering::equivalent == comp_result) ?
					compare(args...) : comp_result;
			}
			else
			{
				return (comp_result == 0) ?
					compare(args...) : comp_result;
			}
		}
	}

	/**
	 * @brief
	 *  Runs a comparison operation on iteratively on each set of two parameters until it
	 *  finds a pair that are not equal and returns the result of that compare operation.
	 */
	template<typename left_t, HasCompareWith<left_t> right_t, typename ...args_t>
	constexpr auto compare(const left_t& left, const right_t& right, const args_t& ...args) noexcept
	{	
		if constexpr ( 0 == sizeof...(args) )
		{
			return left.compare(right);
		}
		else
		{
			const auto comp_result = left.compare(right);

			if ( 0 == comp_result )
				return compare(args...);

			return comp_result;
		}
	}

	/**
	 * @brief
	 *  Runs an equality check on iteratively on each set of two parameters.  If all pairs
	 *  are equal, true is returned.
	 */
	template<EqualityComperable left_t, EqualityComperableWith<left_t> right_t, typename ...args_t>
	constexpr bool equals(const left_t& left, const right_t& right, const args_t& ...args)
	{
		static_assert(sizeof...(args) % 2 == 0);

		if constexpr (0 == sizeof...(args))
		{
			if constexpr (HasCompareWith<left_t, right_t>)
				return (0 == left.compare(right));
			else if constexpr ( HasEqualsWith<left_t, right_t> )
				return (left == right);
			else if constexpr ( ThreeWayComperableWith<left_t, right_t> )
				return 0 == (left <=> right);
			else
				return false;
		}
		else
		{
			return ( equals(left, right) && equals(args...) );
		}
	}

	using comp_cat = std::common_comparison_category_t<std::partial_ordering, std::weak_ordering>;
}

#endif // _STD_EXT_COMPARE_H_
