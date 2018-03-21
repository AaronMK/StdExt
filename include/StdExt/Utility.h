#ifndef _STD_EXT_UTILITY_H_
#define _STD_EXT_UTILITY_H_

#include <typeindex>
#include <type_traits>

namespace StdExt
{
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