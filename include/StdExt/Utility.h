#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <tuple>
#include <limits>
#include <random>
#include <utility>
#include <cmath>
#include <stdexcept>

#include "Type.h"
#include "Concepts.h"
#include "Platform.h"

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
	static constexpr bool isPowerOf2(T number)
	{
		return (0 != number && (number & (number - 1)) == 0);
	}

	template<Integral T>
	static constexpr T nextPowerOf2(T num)
	{
		T v = num - 1;
		if constexpr ( sizeof(T) >= 1 )
		{
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
		}

		if constexpr ( sizeof(T) >= 2 )
			v |= v >> 8;

		if constexpr ( sizeof(T) >= 4 )
			v |= v >> 16;

		if constexpr ( sizeof(T) >= 8 )
			v |= v >> 32;

		return ++v;
	}

	template<Arithmetic T>
	static T nextMutltipleOf(T num, T multiple)
	{
		if constexpr ( Integral<T> )
			return T(std::ceil((float)num / (float)multiple) * (float)multiple);
		else
			return T(std::ceil(num / multiple) * multiple);
	}

	/**
	 * @brief
	 *  Tests for approximate equality of left and right.  For floating point types,
	 *  this will pass if the relative error is within the threshold parameter.
	 *  For integral types, this compiles down to a standard equality test.
	 */
	template<Arithmetic T>
	static bool approxEqual(T left, T right, float threshold = 0.0001)
	{
		if constexpr ( Integral<T> )
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
	 *  Computes a x b - c x d in a numerically stable fashion.
	 */
	template<Arithmetic T>
	T differenceOfProducts(T a, T b, T c, T d)
	{
		if constexpr ( FloatingPoint<T> )
		{
			T cd = c * d;
			return std::fma(a, b, -cd) + std::fma(-c, d, cd);
		}
		else
		{
			return (a * b) - (c * d);
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
		T add(T left, T right)
		{
			if (right < 0)
			{
				if (left < std::numeric_limits<T>::lowest() - right)
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
			if (right < 0)
			{
				if (left > std::numeric_limits<T>::max() + right)
					throw std::overflow_error("Arithmetic Overflow");
			}
			else if (left < std::numeric_limits<T>::lowest() + right)
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
	 *  Runs approximate compare logic for arithmetic types.
	 */
	template<Arithmetic T>
	int approxCompare(T left, T right)
	{
		if ( approxEqual(left, right) )
			return 0;
		if (left < right)
			return -1;
		else
			return 1;
	}

	/**
	 * @brief
	 *  Runs a comparison operation on iteratively on each set of two parameters until it
	 *  finds a pair that are not equal and returns the result of that compare operation.
	 *  This makes it easy to create more complex compare operations for complex types.
	 */
	template<Arithmetic t_a, std::same_as<t_a> t_b, Arithmetic ...args_t>
	int approxCompare(t_a left, const t_b right, args_t ...args)
	{
		static_assert(sizeof...(args) % 2 == 0);

		int comp_result = approxCompare<t_a>(left, right);
		return (comp_result != 0) ? comp_result : approxCompare(args...);
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

	template<Interface T>
	class VTable final
	{
	private:
		alignas( alignof(T) ) char mTable[sizeof(T)];

		#ifdef STD_EXT_DEBUG
			T* mTablePointer;
		#endif

	public:
		VTable(const VTable&) = default;

		VTable()
		{
			memset(mTable, 0, sizeof(T));

			#ifdef STD_EXT_DEBUG
				mTablePointer = (T*)mTable;
			#endif
		}

		template<typename iface_t>
			requires Interface<T> && SuperclassOf<T, iface_t>
		void set()
		{
			new(mTable) iface_t;
		}

		void clear()
		{
			memset(mTable, 0, sizeof(T));
		}

		const T* operator->() const
		{
			return access_as<const T*>(mTable);
		}

		T* operator->()
		{
			return access_as<T*>(mTable);
		}
	};
}

#endif // _STD_EXT_UTILITY_H_