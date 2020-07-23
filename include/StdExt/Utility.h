#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <tuple>
#include <limits>
#include <random>
#include <utility>
#include <cmath>

#include "Type.h"
#include "Concepts.h"

namespace StdExt
{
	template<Arithmetic T>
	T rand(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
	{
		static std::random_device rd;

		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> dist(min, max);
			return dist(rd);
		}
		else
		{
			std::uniform_real_distribution<T> dist(min, max);
			return dist(rd);
		}
	}

	template<Integral T>
	static bool isPowerOf2(T number)
	{
		return (0 != number && (number & (number - 1)) == 0);
	}

	template<Arithmetic T>
	static T nextMutltipleOf(T num, T multiple)
	{
		if constexpr ( Integral<T> )
			return T(std::ceil((float)num / (float)multiple) * (float)multiple);
		else
			return T(std::ceil(num / multiple) * multiple);
	}

	template<Arithmetic T>
	static bool approximately_equal(T left, T right, float threshold = 0.0001)
	{
		static_assert(std::is_arithmetic_v<T>, "T must be a numeric type.");

		if constexpr ( Integral<T> )
		{
			return (left == right);
		}
		else
		{
			constexpr T zero = T(0.0);

			if (left == right)
				return true;

			if (std::isnan(left) && std::isnan(left))
				return false;


			if (left == zero || right == zero)
				return false;

			T relative_error = fabs((left - right) / std::min(left, right));
			return (relative_error <= (T)threshold);
		}
	}

	/**
	 * @brief
	 *  Arithmetic operations that will detect results outside the limits 
	 *  of the type and throw std::underflow_error or std::overflow_error
	 *  exceptions as appropriate.
	 */
	namespace Checked
	{
		template<Arithmetic T>
		T add(T left, T right);

		template<Arithmetic T>
		T subtract(T left, T right);

		template<Arithmetic T>
		T add(T left, T right)
		{
			static_assert(Traits<T>::is_arithmetic);

			if (right < 0)
			{
				if (left < std::numeric_limits<T>::min() - right)
					throw std::underflow_error("Arithmetic Underflow");
			}
			else if (left > std::numeric_limits<T>::max() - right)
			{
				throw std::overflow_error("Arithmetic Overflow");
			}

			return (left + right);
		}

		template<Arithmetic T>
		T subtract(T left, T right)
		{
			static_assert(Traits<T>::is_arithmetic);

			if (right < 0)
			{
				if (left > std::numeric_limits<T>::max() + right)
					throw std::overflow_error("Arithmetic Overflow");
			}
			else if (left < std::numeric_limits<T>::min() + right)
			{
				throw std::underflow_error("Arithmetic Underflow");
			}

			return (left - right);
		}
	}

	/**
	 * @brief
	 *  Updates dest with value, returning true if an update was necessary
	 *  or false if no change was necessary.
	 */
	template<HasNotEqual T>
	static bool update(T& dest, const T& value)
	{
		if (dest != value)
		{
			dest = value;
			return true;
		}

		return false;
	}

	/**
	 * @brief
	 *  Updates dest with value, returning true if an update was necessary
	 *  or false if no change was necessary.
	 */
	template<typename T>
		requires HasNotEqual<T> && MoveAssignable<T>
	static bool update(T& dest, T&& value)
	{
		if (dest != value)
		{
			dest = std::move(value);
			return true;
		}

		return false;
	}

	/**
	 * @brief
	 *  An empty class definition that can be used as a dummy template parameter.
	 */
	class EmptyClass
	{
	};

	/**
	 * @brief
	 *  Casts a pointer, taking care of any necessary constant or other casting needed
	 *  to force the conversion to work.
	 */
	template<Pointer out_t, Pointer in_t>
	out_t force_cast_pointer(in_t ptr)
	{
		using in_ptr_t = Traits<in_t>::pointer_t;
		using out_ptr_t = Traits<out_t>::pointer_t;

		return reinterpret_cast<out_ptr_t>(
			const_cast<in_ptr_t>(ptr)
		);
	}

	/**
	 * @brief
	 *  For debug configurations, this will perform a checked cast and throw errors
	 *  upon failure.  For release configurations, this will be a simple quick
	 *  unchecked cast.
	 */
	template<Pointer out_t, Pointer in_t>
	out_t cast_pointer(in_t ptr)
	{
	#ifdef STDEXT_DEBUG
		out_t ret = dynamic_cast<out_t>(ptr);
		
		if (ret == nullptr && ptr != nullptr)
			throw bad_cast();

		return ret;
	#else
		return reinterpret_cast<out_t>(ptr);
	#endif
	}

	/**
	 * @brief
	 *  Runs a compare operation for any types supporting the less-than, equality, and greater-than operators.
	 */
	template<Comperable t_a, ComperableWith<t_a> t_b>
	int compare(const t_a& left, const t_b& right)
	{
		if (left < right)
			return -1;
		else if (left == right)
			return 0;
		else
			return 1;
	}

	/**
	 * @brief
	 *  Runs a comparison operation on iteratively on each set of two parameters until it
	 *  finds a pair that are not equal and returns the result of that compare operation.
	 *  This makes it easy to create more complex compare operations for complex types.
	 */
	template<Comperable t_a, ComperableWith<t_a> t_b, typename ...args_t>
	int compare(const t_a& left, const t_b& right, const args_t& ...args)
	{
		static_assert(sizeof...(args) % 2 == 0);

		int comp_result = compare<t_a>(left, right);
		return (comp_result != 0) ? comp_result : compare(args...);
	}
}

#endif // _STD_EXT_UTILITY_H_