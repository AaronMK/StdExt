#ifndef _STDEXT_CONCEPTS_H_
#define _STDEXT_CONCEPTS_H_

#include <type_traits>

namespace StdExt
{
#pragma region internal
	template<typename T, typename ...test_rest>
	struct _is_any_of;

	template<typename T, typename test_a>
	struct _is_any_of<T, test_a>
	{
		static constexpr bool value = std::is_same_v<T, test_a>;
	};


	template<typename T, typename test_a, typename test_b>
	struct _is_any_of<T, test_a, test_b>
	{
		static constexpr bool value =
			std::is_same_v<T, test_a> ||
			std::is_same_v<T, test_b>;
	};

	template<typename T, typename test_a, typename test_b, typename ...test_rest>
	struct _is_any_of<T, test_a, test_b, test_rest...>
	{
		static constexpr bool value =
			std::is_same_v<T, test_a> ||
			_is_any_of<T, test_b, test_rest...>::value;
	};
#pragma endregion

	template<typename T, typename ...test_t>
	concept AnyOf = _is_any_of<T, test_t...>::value;

	template<typename T>
	concept Signed = AnyOf<T, int8_t, int16_t, int32_t, int64_t, float, double>;

	template<typename T>
	concept Unsigned = AnyOf<T, uint8_t, uint16_t, uint32_t, uint64_t, unsigned long>;

	template<typename T>
	concept Integral = AnyOf<T, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, long>;

	template<typename T>
	concept FloatingPoint = AnyOf<float, double>;

	template<typename T>
	concept Arithmetic = Signed<T> || Unsigned<T>;

	template<typename T>
	concept DefaultConstructable = std::is_default_constructible_v<T>;

	template<typename T>
	concept MoveConstructable = std::is_move_constructible_v<T>;

	template<typename T>
	concept CopyConstructable = std::is_copy_constructible_v<T>;
}

#endif // !_STDEXT_CONCEPTS_H_