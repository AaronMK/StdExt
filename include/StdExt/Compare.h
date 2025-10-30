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
#include "Operators.h"

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
			requires (sizeof...(remaining_t) % 2 ==  0)
		struct OrderingResultImpl<left_t, right_t, remaining_t...>
		{
			using result_t = std::common_comparison_category_t<
				typename OrderingResultImpl<left_t, right_t>::result_t,
				typename OrderingResultImpl<remaining_t...>::result_t
			>;
		};

		template<typename... comp_types>
		struct UniformOrderingResultImpl;

		template<typename T>
		struct UniformOrderingResultImpl<T>
		{
			using result_t = std::compare_three_way_result_t<T>;
		};

		template<typename first_t, typename second_t>
		struct UniformOrderingResultImpl<first_t, second_t>
		{
			using result_t = std::common_comparison_category_t<
				typename std::compare_three_way_result_t<first_t>,
				typename std::compare_three_way_result_t<second_t>
			>;
		};

		template<typename first_t, typename second_t, typename... remaining_t>
		struct UniformOrderingResultImpl<first_t, second_t, remaining_t...>
		{
			using result_t = std::common_comparison_category_t<
				typename UniformOrderingResultImpl<first_t, second_t>::result_t,
				typename UniformOrderingResultImpl<remaining_t...>::result_t
			>;
		};
	}	

	/**
	 * @brief
	 *  The type is one of the three standard ordering classes.
	 */
	template<typename T>
	concept OrderingClass = AnyOf<T, std::weak_ordering, std::partial_ordering, std::strong_ordering>;

	/**
	 * @brief
	 *  Gets the comparison catagory result type for if pairs of each T parameter were
	 *  used in sequence, as if member types of a structural type.
	 * 
	 * @details
	 * 
	 * As if the following.
	 * 
	 * @code
	 *	struct CompareType
	 *	{
	 *		int         First;
	 *		float       Second;
	 *		std::string Third
	 *		
	 *		constexpr auto operator<=>(const CompareType& other) const = default;
	 *	};
	 *
	 *	// std::compare_three_way_result_t<CompareType>
	 *	//
	 *	// same as
	 *	//
	 *	// UniformOrderingResult<int, float, std::string>
	 */
	template<typename... T>
	using UniformOrderingResult = Detail::UniformOrderingResultImpl<T...>::result_t;

	/**
	 * @brief
	 *  Gets the comparison catagory result type for if each pair of T types
	 *  (T[0] <=> T[1], T[2] <=> T[3], ...) are compared for a common comparison
	 *  catagory.
	 */
	template<typename... T>
	using OrderingResult = Detail::OrderingResultImpl<T...>::result_t;
	
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
	 *  The type has a three-way comparison operator.  It can be of
	 *  any of the three standard ordering catagories.
	 */
	template<typename T>
	concept ThreeWayComperable = requires(const T& left, const T& right)
	{
		{ left <=> right } -> OrderingClass;
	};

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::equals()
	 *  to evaluate equality.
	 */
	template<typename T>
	concept EqualityComperable = 
		HasEquals<T> ||
		ThreeWayComperable<T>; 

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::compare()
	 *  to compare with a value of same type.
	 */
	template<typename T>
	concept Comperable = 
		ThreeWayComperable<T> ||
		(HasLessThan<T> && HasGreaterThan<T>);

	/**
	 * @brief
	 *  The type has a three-way comparison operator with with_t.  It can be of
	 *  any of the three standard ordering categories.
	 */
	template<typename T, typename with_t>
	concept ThreeWayComperableWith = requires(const T& left, const with_t& right)
	{
		{ left <=> right } -> OrderingClass;
	};

	/**
	 * @brief
	 *  The type has a mechanism that can be used by StdExt::equals()
	 *  to evaluate equality with a value of type with_t.
	 */
	template<typename T, typename with_t>
	concept EqualityComperableWith = 
		HasEqualsWith<T, with_t> ||
		ThreeWayComperableWith<T, with_t>;

	/**
	 * @brief
	 *  Less than comparison that uses the three-way comparison if the standard operator
	 *  does not return bool.
	 */
	template<typename left_t, typename right_t>
	static constexpr bool isLessThan(const left_t& left, const right_t& right)
	{
		using result_t = LessThan<left_t, right_t>::result_type;
		static_assert(
			ThreeWayComperableWith<left_t, right_t> || std::same_as<result_t, bool>,
			"left_t and right_t must be comperable."
		);

		if constexpr ( std::same_as<result_t, bool> )
			return left < right;
		else
			return std::is_lt( left <=> right );
	}

	/**
	 * @brief
	 *  Less than or equal comparison that uses the three-way comparison if the standard operator
	 *  does not return bool.
	 */
	template<typename left_t, typename right_t>
	static constexpr bool isLessThanEqual(const left_t& left, const right_t& right)
	{
		using result_t = LessThanEqual<left_t, right_t>::result_type;
		static_assert(
			ThreeWayComperableWith<left_t, right_t> || std::same_as<result_t, bool>,
			"left_t and right_t must be comperable."
		);

		if constexpr ( std::same_as<result_t, bool> )
			return left <= right;
		else
			return std::is_lteq( left <=> right );
	}

	/**
	 * @brief
	 *  Equality comparison that uses the three-way comparison if the standard operator
	 *  does not return bool.
	 */
	template<typename left_t, typename right_t>
	static constexpr bool isEqual(const left_t& left, const right_t& right)
	{
		using result_t = Equal<left_t, right_t>::result_type;
		static_assert(
			ThreeWayComperableWith<left_t, right_t> || std::same_as<result_t, bool>,
			"left_t and right_t must be comperable."
		);

		if constexpr ( std::same_as<result_t, bool> )
			return left == right;
		else
			return std::is_eq( left <=> right );
	}

	/**
	 * @brief
	 *  Non-equality comparison that uses the three-way comparison if the standard operator
	 *  does not return bool.
	 */
	template<typename left_t, typename right_t>
	static constexpr bool isNotEqual(const left_t& left, const right_t& right)
	{
		using result_t = NotEqual<left_t, right_t>::result_type;
		static_assert(
			ThreeWayComperableWith<left_t, right_t> || std::same_as<result_t, bool>,
			"left_t and right_t must be comperable."
		);

		if constexpr ( std::same_as<result_t, bool> )
			return left != right;
		else
			return std::is_neq( left <=> right );
	}

	/**
	 * @brief
	 *  Greater than or equal comparison that uses the three-way comparison if the standard operator
	 *  does not return bool.
	 */
	template<typename left_t, typename right_t>
	static constexpr bool isGreaterThanEqual(const left_t& left, const right_t& right)
	{
		using result_t = GreaterThanEqual<left_t, right_t>::result_type;
		static_assert(
			ThreeWayComperableWith<left_t, right_t> || std::same_as<result_t, bool>,
			"left_t and right_t must be comperable."
		);

		if constexpr ( std::same_as<result_t, bool> )
			return left >= right;
		else
			return std::is_gteq( left <=> right );
	}

	/**
	 * @brief
	 *  Greater than comparison that uses the three-way comparison if the standard operator
	 *  does not return bool.
	 */
	template<typename left_t, typename right_t>
	static constexpr bool isGreaterThan(const left_t& left, const right_t& right)
	{
		using result_t = GreaterThan<left_t, right_t>::result_type;
		static_assert(
			ThreeWayComperableWith<left_t, right_t> || std::same_as<result_t, bool>,
			"left_t and right_t must be comperable."
		);

		if constexpr ( std::same_as<result_t, bool> )
			return left > right;
		else
			return std::is_gt( left <=> right );
	}

	template<Arithmetic T>
	static constexpr double relativeDifference(T left, T right)
	{
		if (left == right)
			return 0.0;

		const double d_left  = static_cast<double>(left);
		const double d_right = static_cast<double>(right);
		const double d_min   = std::min(d_left, d_right);

		if ( 0.0 == d_min)
			return std::numeric_limits<double>::quiet_NaN();

		return std::abs((d_left - d_right) / d_min);
	}

	/**
	 * @brief
	 *  Tests for approximate equality of left and right.  For floating point types,
	 *  this will pass if the relative error is within the threshold parameter.
	 *  For integral types, this compiles down to a standard equality test.
	 */
	template<Arithmetic left_t, Arithmetic right_t>
	static constexpr bool approxEqual(left_t left, right_t right, float threshold = 0.0001)
	{
		if constexpr (Integral<left_t> && Integral<right_t>)
		{
			return (left == right);
		}
		else if constexpr (Integral<left_t> || Integral<right_t>)
		{
			return approxEqual(
				static_cast<double>(left),
				static_cast<double>(right)
			);
		}
		else
		{
			using float_t = std::common_type_t<left_t, right_t>;
			constexpr float_t zero = static_cast<float_t>(0.0);

			if (left == right)
				return true;

			if (left == zero || right == zero)
				return false;

			if (std::isnan(left) && std::isnan(right))
				return false;

			const float_t relative_error = std::abs((left - right) / std::min<float_t>(left, right));
			return (relative_error <= static_cast<float_t>(threshold));
		}
	}

	/**
	 * @brief
	 *  Runs approximate compare logic for arithmetic types.
	 */
	template<Arithmetic left_t, Arithmetic right_t>
	constexpr auto approxCompare(const left_t& left, const right_t& right)
	{
		if constexpr ( FloatingPoint<left_t> || FloatingPoint<right_t> )
		{
			if ( approxEqual<float64_t>(left, right) )
				return std::partial_ordering::equivalent;
			else
				return left <=> right;
		}
		else
		{
			return left <=> right;
		}
	}

	/**
	 * @brief
	 *  Runs approximate compare logic iteratively on each set of two parameters until it
	 *  finds a pair that are not equal and returns the result of that compare operation.
	 *  If all pairs are equal, zero is returned.
	 */
	template<Arithmetic t_a, std::same_as<t_a> t_b, Arithmetic ...args_t>
	constexpr auto approxCompare(t_a left, const t_b right, args_t ...args)
	{
		static_assert(sizeof...(args) % 2 == 0);

		auto comp_result = approxCompare<t_a>(left, right);

		if ( std::is_neq(comp_result) )
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
	template<typename left_t, typename right_t, typename ...args_t>
	constexpr auto compare(const left_t& left, const right_t& right, const args_t& ...args) noexcept
	{
		static_assert(sizeof...(args) % 2 == 0, "Agrument count must be a multiple of 2.");
		static_assert( OrderingClass< OrderingResult<left_t, right_t, args_t...> >, "Each pair of types must support ordering.");
		using return_t = OrderingResult<left_t, right_t, args_t...>;


		if constexpr ( 0 == sizeof...(args_t) )
		{
			return static_cast<return_t>(left <=> right);
		}
		else
		{
			const auto comp_result = left <=> right;

			if ( std::is_eq(comp_result) )
			{
				return static_cast<return_t>( compare(args...) );
			}
			
			return static_cast<return_t>(comp_result);
		}
	}
}

#endif // _STD_EXT_COMPARE_H_
