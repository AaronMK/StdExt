/**
 * @file
 *
 * @brief
 *  A combination of conceptualizing existing type traits and tests in the STL,
 *  adding additions, or redefining the concept if has a characteristic
 *  that is counter intuitive to its purpose (for example, considering bool a
 *  numeric value just because it can be implicitly converted).
 */

#ifndef _STDEXT_CONCEPTS_H_
#define _STDEXT_CONCEPTS_H_

#include <type_traits>
#include <functional>
#include <concepts>

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
	
	//////////////////////////////
	
	template<typename T, typename ...types_t>
	struct _assignable_from;

	template <typename to_t, typename from_t>
	struct _assignable_from<to_t, from_t>
	{
		static constexpr bool value = std::is_convertible_v<to_t, from_t>;
	};

	template <typename t_a, typename t_b, typename ...type_rest>
	struct _assignable_from<t_a, t_b, type_rest...>
	{
		static constexpr bool value = 
			std::is_convertible_v<t_a, t_b> &&
			std::is_convertible_v<t_a, type_rest...>;
	};

#pragma endregion

	template<typename T>
	concept Pointer = std::is_pointer_v<T>;

	template<typename T>
	concept Class = std::is_class_v<T>;

	/**
	 * @brief
	 *  Passes if T is any one of the remaining types passed.
	 */
	template<typename T, typename ...test_t>
	concept AnyOf = _is_any_of<T, test_t...>::value;

	/**
	 * @brief
	 *  Passes if T is a scaler signed type.
	 */
	template<typename T>
	concept Signed = AnyOf<T, int8_t, int16_t, int32_t, int64_t, float, double>;

	/**
	 * @brief
	 *  Passes if T is a scaler unsigned type.
	 */
	template<typename T>
	concept Unsigned = AnyOf<T, uint8_t, uint16_t, uint32_t, uint64_t, unsigned long>;

	/**
	 * @brief
	 *  Passes if T is a scaler Integral type.
	 */
	template<typename T>
	concept Integral = AnyOf<T, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, long>;

	/**
	 * @brief
	 *  Passes if T is a scaler floating point type.
	 */
	template<typename T>
	concept FloatingPoint = AnyOf<float, double>;

	/**
	 * @brief
	 *  Passes if T is a scaler numeric type.
	 */
	template<typename T>
	concept Arithmetic = Signed<T> || Unsigned<T>;

	/**
	 * @brief
	 *  Passes if T is an enum type.
	 */
	template<typename T>
	concept Enumeration = std::is_enum_v<T>;

	/**
	 * @brief
	 *  Passes if T is a scaler or "primitive", or "fundamental" type.
	 */
	template<typename T>
	concept Scaler = 
		std::is_same_v<T, bool> || std::is_pointer_v<T> || std::is_enum_v<T> ||
		Signed<T> || Unsigned<T>;

	template<typename T>
	concept DefaultConstructable = std::is_default_constructible_v<T>;

	/**
	 * @brief
	 *  Passes if the type has a good notion of a default value.  For numerics, this
	 *  would be zero, for bool it would be false, and for classes it would be the
	 *  default constructor.
	 *
	 * @see StdExt::Traits<T>::default_value()
	 */
	template<typename T>
	concept Defaultable = DefaultConstructable<T> || Scaler<T>;

	template<typename T>
	concept MoveConstructable = std::is_move_constructible_v<T>;

	template<typename T>
	concept MoveAssignable = std::is_move_assignable_v<T>;

	template<typename T>
	concept CopyAssignable = std::is_copy_assignable_v<T>;

	template<typename T>
	concept CopyConstructable = std::is_copy_constructible_v<T>;

	/**
	 * @brief
	 *  Passes if T is assignable from all of the provided types.
	 */
	template<typename T, typename ...args_t>
	concept AssignableFrom = _assignable_from<T, args_t...>::value;

	template<typename T, typename with_t>
	concept HasLessThanWith = requires (T L, with_t R)
	{
		{ L < R } -> std::same_as<bool>;
	};

	template<typename T, typename with_t>
	concept HasLessThanEqualWith = requires (T L, with_t R)
	{
		{ L <= R } -> std::same_as<bool>;
	};

	template<typename T, typename with_t>
	concept HasEqualsWith = requires (T L, with_t R)
	{
		{ L == R } -> std::same_as<bool>;
	};

	template<typename T, typename with_t>
	concept HasNotEqualWith = requires (T L, with_t R)
	{
		{ L != R } -> std::same_as<bool>;
	};

	template<typename T, typename with_t>
	concept HasGreaterThanEqualWith = requires (T L, with_t R)
	{
		{ L >= R } -> std::same_as<bool>;
	};

	template<typename T, typename with_t>
	concept HasGreaterThanWith = requires (T L, with_t R)
	{
		{ L > R } -> std::same_as<bool>;
	};

	template<typename T, typename with_t>
	concept ComperableWith = HasLessThanWith<T, with_t> && HasEqualsWith<T, with_t> && HasGreaterThanWith<T, with_t>;

	template<typename T>
	concept HasLessThan = HasLessThanWith<T, T>;

	template<typename T>
	concept HasLessThanEqual = HasLessThanEqualWith<T, T>;

	template<typename T>
	concept HasEquals = HasEqualsWith<T, T>;

	template<typename T>
	concept HasNotEqual = HasNotEqualWith<T, T>;

	template<typename T>
	concept HasGreaterThanEqual = HasGreaterThanEqualWith<T, T>;

	template<typename T>
	concept HasGreaterThan = HasGreaterThanWith<T, T>;

	template<typename T>
	concept Comperable = HasLessThan<T> && HasEquals<T> && HasGreaterThan<T>;

	template<typename T>
	concept HasAnd = std::is_same_v<
		std::invoke_result_t<std::logical_and<>, T, T>,
		bool
	>;

	template<typename T>
	concept HasOr = std::is_same_v<
		std::invoke_result_t<std::logical_or<>, T, T>,
		bool
	>;

	template<typename T>
	concept HasNot = std::is_same_v<
		std::invoke_result_t<std::logical_not<>, T, T>,
		bool
	>;
}

#endif // !_STDEXT_CONCEPTS_H_