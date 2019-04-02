#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <tuple>
#include <limits>
#include <random>
#include <utility>
#include <cmath>

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
		return T(std::ceil((double)num/(double)multiple)*(double)multiple);
	}

	/**
	 * @brief
	 *  Tests if the callable func_t can be invoked args_t parameters and provide a result that is
	 *  convertable to result_t.
	 */
	template<typename result_t, typename func_t, typename ...args_t>
	struct can_invoke
	{
		template<typename r_t, typename f_t, typename ...a_t>
		static std::is_convertible<std::invoke_result_t<f_t, a_t...>, r_t> test_func(int) {};

		template<typename r_t, typename f_t, typename ...a_t>
		static std::false_type test_func(...) {};

		static constexpr bool value = decltype(test_func<result_t, func_t, args_t...>(0))::value;
	};

	/**
	 * @brief
	 *  Tests if the callable func_t can be invoked args_t parameters and provide a result that is
	 *  convertable to result_t.
	 */
	template<typename result_t, typename func_t, typename ...args_t>
	constexpr bool can_invoke_v =  can_invoke<result_t, func_t, args_t...>::value;
}

#endif // _STD_EXT_UTILITY_H_