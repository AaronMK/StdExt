#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <tuple>
#include <limits>
#include <random>
#include <utility>
#include <cmath>

#include "Type.h"

namespace StdExt
{
	template<typename T>
	T rand(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
	{
		static_assert(std::is_arithmetic_v<T>);
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

	template<typename T>
	static bool isPowerOf2(T number)
	{
		static_assert(std::is_integral<T>::value, "Integral type required.");
		return (0 != number && (number & (number - 1)) == 0);
	}

	template<typename T>
	static T nextMutltipleOf(T num, T multiple)
	{
		static_assert(std::is_arithmetic_v<T>, "T must be a numeric type.");

		if constexpr (std::is_integral_v<T>)
			return T(std::ceil((float)num / (float)multiple) * (float)multiple);
		else
			return T(std::ceil(num / multiple) * multiple);
	}

	template<typename T>
	static bool approximately_equal(T left, T right, float threshold = 0.0001)
	{
		static_assert(std::is_arithmetic_v<T>, "T must be a numeric type.");

		if constexpr (std::is_integral_v<T>)
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


			if (left == zero || right == 0)
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
		template<typename T>
		T add(T left, T right);

		template<typename T>
		T subtract(T left, T right);

		template<typename T>
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

		template<typename T>
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
	template<typename T>
	static bool update(T& dest, const T& value)
	{
		static_assert(
			Traits<T>::has_inequality&& Traits<T>::copy_assignable,
			"T must support inequality testing and be copy assignable."
		);

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
	static bool update(T& dest, T&& value)
	{
		static_assert(
			Traits<T>::has_inequality && Traits<T>::move_assignable,
			"T must support inequality testing and be move assignable."
		);

		if (dest != value)
		{
			dest = std::move(value);
			return true;
		}

		return false;
	}
}

#endif // _STD_EXT_UTILITY_H_