#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <type_traits>

namespace StdExt
{
	template<typename T>
	bool isPowerOf2(T number)
	{
		static_assert(std::is_integral<T>::value, "Integral type required.");
		return (0 != number && (number & (number - 1)) == 0);
	}
}

#endif // _STD_EXT_UTILITY_H_