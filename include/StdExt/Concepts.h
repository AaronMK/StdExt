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

#include "StdExt.h"
#include "Type.h"

#include <type_traits>
#include <functional>
#include <concepts>

namespace StdExt
{
#pragma region internal

#	pragma region _PointerType
	template<typename ...args_t>
	struct _PointerType;

	/**
	 * @internal
	 */
	template<typename T>
	struct _PointerType<T>
	{
		static constexpr bool value = std::is_pointer_v<T>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _PointerType<t_a, t_b>
	{
		static constexpr bool value = _PointerType<t_a>::value && _PointerType<t_b>::value;
	};

	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _PointerType<t_a, t_b, test_rest...>
	{
		static constexpr bool value = _PointerType<t_a>::value && _PointerType<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _PointerType_v = _PointerType<args_t...>::value;
#	pragma endregion

#	pragma region _ReferenceType
	template<typename ...args_t>
	struct _ReferenceType;

	/**
	 * @internal
	 */
	template<typename T>
	struct _ReferenceType<T>
	{
		static constexpr bool value = std::is_reference_v<T>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _ReferenceType<t_a, t_b>
	{
		static constexpr bool value = _ReferenceType<t_a>::value && _ReferenceType<t_b>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _ReferenceType<t_a, t_b, test_rest...>
	{
		static constexpr bool value = _ReferenceType<t_a>::value && _ReferenceType<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _ReferenceType_v = _ReferenceType<args_t...>::value;
#	pragma endregion

#	pragma region _ConstType
	template<typename ...args_t>
	struct _ConstType;

	/**
	 * @internal
	 */
	template<typename T>
	struct _ConstType<T>
	{
		using strp_t = typename Type<T>::stripped_ptr_ref_t;
		static constexpr bool value = 
			std::is_const_v<strp_t> ||
			std::is_same_v<const T, strp_t>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _ConstType<t_a, t_b>
	{
		static constexpr bool value = _ConstType<t_a>::value && _ConstType<t_b>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _ConstType<t_a, t_b, test_rest...>
	{
		static constexpr bool value = _ConstType<t_a>::value && _ConstType<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _ConstType_v = _ConstType<args_t...>::value;
#	pragma endregion

#	pragma region _NonConstType
	template<typename ...args_t>
	struct _NonConstType;

	template<typename T>
	struct _NonConstType<T>
	{
		static constexpr bool value = !_ConstType<T>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _NonConstType<t_a, t_b>
	{
		static constexpr bool value = _NonConstType<t_a>::value && _NonConstType<t_b>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _NonConstType<t_a, t_b, test_rest...>
	{
		static constexpr bool value = _NonConstType<t_a>::value && _NonConstType<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _NonConstType_v = _NonConstType<args_t...>::value;
#	pragma endregion

#	pragma region _MoveReferenceType
	template<typename ...args_t>
	struct _MoveReferenceType;

	/**
	 * @internal
	 */
	template<typename T>
	struct _MoveReferenceType<T>
	{
		static constexpr bool value = std::is_rvalue_reference_v<T>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _MoveReferenceType<t_a, t_b>
	{
		static constexpr bool value = std::is_rvalue_reference_v<t_a> && std::is_rvalue_reference_v<t_b>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _MoveReferenceType<t_a, t_b, test_rest...>
	{
		static constexpr bool value = std::is_rvalue_reference_v<t_a> && _MoveReferenceType<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _MoveReferenceType_v = _MoveReferenceType<args_t...>::value;
#	pragma endregion

#	pragma region _Class
	template<typename ...args_t>
	struct _Class;

	/**
	 * @internal
	 */
	template<typename T>
	struct _Class<T>
	{
		static constexpr bool value = std::is_class_v<T>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _Class<t_a, t_b>
	{
		static constexpr bool value = std::is_class_v<t_a> && std::is_class_v<t_b>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _Class<t_a, t_b, test_rest...>
	{
		static constexpr bool value = std::is_class_v<t_a> && _Class<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _Class_v = _Class<args_t...>::value;
#	pragma endregion

#	pragma region _Final
	template<typename ...args_t>
	struct _Final;

	/**
	 * @internal
	 */
	template<typename T>
	struct _Final<T>
	{
		static constexpr bool value = std::is_final_v<T>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b>
	struct _Final<t_a, t_b>
	{
		static constexpr bool value = std::is_final_v<t_a> && std::is_final_v<t_b>;
	};
	
	/**
	 * @internal
	 */
	template<typename t_a, typename t_b, typename ...test_rest>
	struct _Final<t_a, t_b, test_rest...>
	{
		static constexpr bool value = std::is_final_v<t_a> && _Final<t_b, test_rest...>::value;
	};
	
	/**
	 * @internal
	 */
	template<typename ...args_t>
	constexpr bool _Final_v = _Final<args_t...>::value;
#	pragma endregion

#pragma region _AssignableFrom

	/**
	 * @internal
	 */
	template <typename T, typename from_t>
	concept _AssignableFrom_concept = requires (T to, from_t from)
	{
		to = from;
	};

	/**
	 * @internal
	 */
	template<typename T, typename ...types_t>
	struct _AssignableFrom;

	/**
	 * @internal
	 */
	template <typename T, typename from_t>
	struct _AssignableFrom<T, from_t>
	{
		static constexpr bool value = _AssignableFrom_concept<T, from_t>;
	};

	/**
	 * @internal
	 */
	template <typename T, typename from_a, typename from_b>
	struct _AssignableFrom<T, from_a, from_b>
	{
		static constexpr bool value = 
			_AssignableFrom<T, from_a>::value &&
			_AssignableFrom<T, from_b>::value;
	};

	/**
	 * @internal
	 */
	template <typename to_t, typename t_a, typename t_b, typename ...type_rest>
	struct _AssignableFrom<to_t, t_a, t_b, type_rest...>
	{
		static constexpr bool value = 
			_AssignableFrom<to_t, t_a>::value &&
			_AssignableFrom<to_t, t_b, type_rest...>::value;
	};

	template<typename to_t, typename ...types_t>
	constexpr bool _AssignableFrom_v = _AssignableFrom<to_t, types_t...>::value;
#pragma endregion

#pragma region _ImplicitlyConvertableTo

	template<typename T>
	class _ImplicitlyConvertableTo_func
	{
	public:
		void operator()(T val) {};
	};

	/**
	 * @internal
	 */
	template <typename T, typename from_t>
	concept _ImplicitlyConvertableTo_concept = requires (_ImplicitlyConvertableTo_func<T> func, from_t from)
	{
		func(from);
	};

	/**
	 * @internal
	 */
	template<typename T, typename ...types_t>
	struct _ImplicitlyConvertableTo;

	/**
	 * @internal
	 */
	template <typename T, typename from_t>
	struct _ImplicitlyConvertableTo<T, from_t>
	{
		static constexpr bool value = _ImplicitlyConvertableTo_concept<T, from_t>;
	};

	/**
	 * @internal
	 */
	template <typename T, typename from_t>
		requires std::is_rvalue_reference_v<T> && std::is_rvalue_reference_v<from_t>
	struct _ImplicitlyConvertableTo<T, from_t>
	{
		static constexpr bool value = 
			std::is_base_of_v<
				Type<T>::stripped_t,
				Type<from_t>::stripped_t
			>;
	};

	/**
	 * @internal
	 */
	template <typename T, typename from_a, typename from_b>
	struct _ImplicitlyConvertableTo<T, from_a, from_b>
	{
		static constexpr bool value = 
			_ImplicitlyConvertableTo<T, from_a>::value &&
			_ImplicitlyConvertableTo<T, from_b>::value;
	};

	/**
	 * @internal
	 */
	template <typename to_t, typename from_a, typename from_b, typename ...type_rest>
	struct _ImplicitlyConvertableTo<to_t, from_a, from_b, type_rest...>
	{
		static constexpr bool value = 
			_ImplicitlyConvertableTo<to_t, from_a>::value &&
			_ImplicitlyConvertableTo<to_t, from_b, type_rest...>::value;
	};

	template<typename to_t, typename ...types_t>
	constexpr bool _ImplicitlyConvertableTo_v = _ImplicitlyConvertableTo<to_t, types_t...>::value;
#pragma endregion

#	pragma region _is_any_of
	/**
	 * @internal
	 */
	template<typename T, typename ...test_rest>
	struct _is_any_of;

	/**
	 * @internal
	 */
	template<typename T, typename test_a>
	struct _is_any_of<T, test_a>
	{
		static constexpr bool value = std::is_same_v<T, test_a>;
	};

	/**
	 * @internal
	 */
	template<typename T, typename test_a, typename test_b>
	struct _is_any_of<T, test_a, test_b>
	{
		static constexpr bool value =
			std::is_same_v<T, test_a> ||
			std::is_same_v<T, test_b>;
	};

	/**
	 * @internal
	 */
	template<typename T, typename test_a, typename test_b, typename ...test_rest>
	struct _is_any_of<T, test_a, test_b, test_rest...>
	{
		static constexpr bool value =
			std::is_same_v<T, test_a> ||
			_is_any_of<T, test_b, test_rest...>::value;
	};
#	pragma endregion

#	pragma region _interface_test
	/**
	 * @internal
	 */
	class _interface_test
	{
	public:
		virtual ~_interface_test() {}
	};
#	pragma endregion

#pragma endregion

	/**
	 * @brief
	 *  Passes if all of the passed types are pointer types.
	 */
	template<typename ...args_t>
	concept PointerType = _PointerType_v<args_t...>;
	
	/**
	 * @brief
	 *  Passes if all of the passed types are reference types.
	 */
	template<typename ...args_t>
	concept ReferenceType = _ReferenceType_v<args_t...>;
	
	/**
	 * @brief
	 *  Passes if all of the passed types provide only const access to
	 *  the values of the type passed.
	 */
	template<typename ...args_t>
	concept ConstType = _ConstType_v<args_t...>;
	
	/**
	 * @brief
	 *  Passes if all of the passed types provide non-const access to
	 *  the values of the type passed.
	 */
	template<typename ...args_t>
	concept NonConstType = _NonConstType_v<args_t...>;

	template<typename ...args_t>
	concept ConstReferenceType = ConstType<args_t...> && ReferenceType<args_t...>;

	template<typename ...args_t>
	concept MoveReferenceType = _MoveReferenceType_v<args_t...>;

	template<typename ...args_t>
	concept Class = _Class_v<args_t...>;

	template<typename ...args_t>
	concept Final = _Final_v<args_t...>;

	template<typename T>
	concept Polymorphic = std::is_polymorphic_v<T>;

	/**
	 * Passes if T has no member data of its own and is a polymophic type.  It means it is likely
	 * to only define functions.
	 */
	template<typename T>
	concept Interface = 
		(sizeof(T) <= sizeof(_interface_test)) && std::is_class_v<T> &&
		std::is_polymorphic_v<T>;

	/**
	 * @brief
	 *  Passes if T is the same class as or is derived from super_t.
	 */
	template<typename T, typename super_t>
	concept SubclassOf = std::is_base_of_v<super_t, T>;

	/**
	 * @brief
	 *  Passes if the type is the exact same as the tested type.
	 */
	template<typename T, typename test_t>
	concept Is = std::same_as<T, test_t>;

	/**
	 * @brief
	 *  Passes if the type is not the exact same as the tested type.
	 */
	template<typename T, typename test_t>
	concept IsNot = !std::same_as<T, test_t>;
	/**
	 * @brief
	 *  Passes if T is the same class as or is a superclass of sub_t.
	 */
	template<typename T, typename sub_t>
	concept SuperclassOf = std::is_base_of_v<T, sub_t>;

	/**
	 * @brief
	 *  Passes if T is either the same, a sub, or a super class of test_t.
	 */
	template<typename T, typename test_t>
	concept InHeirarchyOf = SubclassOf<T, test_t> || SuperclassOf<T, test_t>;

	/**
	 * @brief
	 *  Passes if T is a type without an pointer, reference, or const qualifiers.
	 */
	template<typename T>
	concept StrippedType = !std::is_pointer_v<T> && !std::is_reference_v<T> && !std::is_const_v<T>;

	/**
	 * @brief
	 *  Passes if T is a type that can be safely moved with a memory move operation.
	 */
	template<typename T>
	concept MemMovable = std::is_trivially_move_constructible_v<T> && std::is_trivially_destructible_v<T>;

	/**
	 * @brief
	 *  Passes if T is a type that can be safely copied with a memory copy operation.
	 */
	template<typename T>
	concept MemCopyable = std::is_trivially_copy_constructible_v<T>;

	/**
	 * @brief
	 *  Passes if T is any one of the remaining types passed.
	 */
	template<typename T, typename ...test_t>
	concept AnyOf = _is_any_of<T, test_t...>::value;

	/**
	 * @brief
	 *  Passes if the T is a fixed width signed number.
	 */
	template<typename T>
	concept FixedWidthSigned = AnyOf<T, int8_t, int16_t, int32_t, int64_t, float32_t, float64_t>;

	/**
	 * @brief
	 *  Passes if the T is a fixed width unsigned number.
	 */
	template<typename T>
	concept FixedWidthUnsigned = AnyOf<T, uint8_t, uint16_t, uint32_t, uint64_t>;

	/**
	 * @brief
	 *  Passes if the T is a fixed width number.
	 */
	template<typename T>
	concept FixedWidthArithmetic = FixedWidthSigned<T> || FixedWidthUnsigned<T>;

	/**
	 * @brief
	 *  Passes if T is a scaler floating point type.
	 */
	template<typename T>
	concept FloatingPoint = AnyOf<T, float, double, float32_t, float64_t>;

	/**
	 * @brief
	 *  Passes if T is a scaler signed type.
	 */
	template<typename T>
	concept Signed =
		AnyOf<T, char, short, int, long, long long, float, double> ||
		FixedWidthSigned<T>;

	/**
	 * @brief
	 *  Passes if T is a scaler unsigned type.
	 */
	template<typename T>
	concept Unsigned =
		AnyOf<T, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, size_t> ||
		FixedWidthUnsigned<T>;

	/**
	 * @brief
	 *  Passes if T is a scaler integral type.
	 */
	template<typename T>
	concept Integral = (Signed<T> || Unsigned<T>) && !FloatingPoint<T>;

	/**
	 * @brief
	 *  Passes if T is a scaler signed integral type.
	 */
	template<typename T>
	concept SignedIntegral = Signed<T> && !FloatingPoint<T>;

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
		Arithmetic<T>;

	template<typename T>
	concept Character = 
		std::is_same_v<T, char> || std::is_same_v<T, char8_t> ||
		std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t> ||
		std::is_same_v<T, wchar_t>;

	/**
	 * @brief
	 *  Passes if T has a default constructor or is a scaler type that is not an enumeration.
	 */
	template<typename T>
	concept DefaultConstructable = std::is_default_constructible_v<T> && !std::is_enum_v<T>;

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
	concept AssignableFrom = _AssignableFrom_v<T, args_t...>;

	/**
	 * @brief
	 *  Passes if T is assignable from all of the provided types.
	 */
	template<typename T, typename ...args_t>
	concept ImplicitlyConvertableTo = _ImplicitlyConvertableTo_v<T, args_t...>;

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

	/**
	 * @brief
	 *  This is a concept that will not pass for any type.  It can
	 *  be used in templates for pre-declarations that will declare the
	 *  template for use in other concepts, but will not be resolved itself.
	 */
	template<typename T>
	concept TypeParam = std::is_class_v<T> && std::is_scalar_v<T>;

	/**
	 * @brief
	 *  Passes if test_t is the same as T when reference, pointer, and const qualifiers
	 *  are removed from test_t.
	 */
	template<typename T, typename test_t>
	concept DecaysTo = std::is_same_v<T, typename Type<test_t>::stripped_t>;

	template<typename T, typename test_t>
	concept ExplicitConstRef = 
		DecaysTo<T, test_t> && 
		(std::is_same_v<T, test_t> || (ConstType<test_t> && ReferenceType<test_t> ));
}

#endif // !_STDEXT_CONCEPTS_H_