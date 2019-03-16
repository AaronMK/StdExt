#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <tuple>
#include <limits>
#include <random>
#include <typeindex>
#include <type_traits>

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
	bool isPowerOf2(T number)
	{
		static_assert(std::is_integral<T>::value, "Integral type required.");
		return (0 != number && (number & (number - 1)) == 0);
	}

	template<typename T>
	std::type_index getTypeIndex()
	{
		return std::type_index(typeid(T));
	}
}

#endif // _STD_EXT_UTILITY_H_