#ifndef _STD_EXT_TYPE_H_
#define _STD_EXT_TYPE_H_

#include <typeindex>
#include <type_traits>
#include <utility>
#include <memory>
#include <tuple>

namespace StdExt
{
	using float32_t = float;
	static_assert(sizeof(float32_t) == 4, "float32_t type must be 32 bits wide.");

	using float64_t = double;
	static_assert(sizeof(float64_t) == 8, "float64_t type must be 64 bits wide.");

	template<size_t index, typename... types>
	using nth_type_t = typename std::tuple_element<index, std::tuple<types...>>::type;

	/**
	 * @brief
	 *  Provides transformations of types.  This is a combination of grouping
	 *  functionality in the standard library and new type related functionality.  This also
	 *  provides type definitions that are useful for templated code.
	 */
	template<typename T>
	struct Type
	{
	private:
		template<typename inner_t, bool more>
		struct Strip;

		template<typename inner_t>
		struct Strip<inner_t, false>
		{
			using type = inner_t;
		};

		template<typename inner_t>
		struct Strip<inner_t, true>
		{
			using stripped = std::remove_cv_t<
				std::remove_pointer_t<
					std::remove_reference_t<inner_t>
				>
			>;

			using type = typename Strip<
				stripped,
				std::is_lvalue_reference_v<stripped> ||
				std::is_rvalue_reference_v<stripped> ||
				std::is_pointer_v<stripped> ||
				std::is_const_v<stripped>
			>::type;
		};

		/////////////////////////////
		
		template<typename inner_t, bool more>
		struct StripRefPtr;

		template<typename inner_t>
		struct StripRefPtr<inner_t, false>
		{
			using type = inner_t;
		};

		template<typename inner_t>
		struct StripRefPtr<inner_t, true>
		{
			using stripped = 
				std::remove_pointer_t<
					std::remove_reference_t<inner_t>
				>;

			using type = typename StripRefPtr<
				stripped,
				std::is_reference_v <stripped> ||
				std::is_pointer_v<stripped>
			>::type;
		};

	public:

		/**
		 * @brief
		 *  Convenience function to get a type_index.
		 */
		static std::type_index index()
		{
			return std::type_index(typeid(T));
		}

		/**
		 * @brief
		 *  True if the type does not have any constant, volitile, reference, or pointer specifiers.
		 *
		 * @code
		 * 	StdExt::Traits<int>::is_stripped;         // true
		 * 	StdExt::Traits<int*>::is_stripped;        // false
		 * 	StdExt::Traits<const int>::is_stripped;   // false
		 *
		 * 	StdExt::Traits<std::u8string>::is_stripped;           // true
		 * 	StdExt::Traits<std::u8string*>::is_stripped;          // false
		 * 	StdExt::Traits<const std::u8string>::is_stripped;     // false
		 *
		 * 	StdExt::Traits< std::vector<std::u8string> >::is_stripped; // true
		 * @endcode
		 */
		static constexpr bool is_stripped = 
			!( std::is_lvalue_reference_v<T> || std::is_pointer_v<T> ||
			   std::is_const_v<T> || std::is_rvalue_reference_v<T> );

		
		static constexpr bool is_ptr_ref_stripped = 
			!( std::is_lvalue_reference_v<T> || std::is_pointer_v<T> ||
			   std::is_rvalue_reference_v<T> );

		/**
		 * @brief
		 *  The type stripped of any constant, volatile, reference, or pointer specifiers.
		 *
		 * @code
		 * 	StdExt::Traits<int>::stripped_t;         // int
		 * 	StdExt::Traits<int*>::stripped_t;        // int
		 * 	StdExt::Traits<const int>::stripped_t;   // int
		 *
		 * 	StdExt::Traits<std::u8string>::stripped_t;           // std::u8string
		 * 	StdExt::Traits<std::u8string*>::stripped_t;          // std::u8string
		 * 	StdExt::Traits<const std::u8string>::stripped_t;     // std::u8string
		 *
		 * 	StdExt::Traits< std::vector<std::u8string> >::stripped_t;  // std::vector<std::u8string>
		 * @endcode
		 */
		using stripped_t = typename Strip<T, !is_stripped>::type;
		using stripped_ptr_ref_t = typename StripRefPtr<T, !is_ptr_ref_stripped>::type;

		using pointer_t = stripped_t*;
		using const_pointer_t = const stripped_t*;
		

		using move_t = stripped_t&&;
		using reference_t = stripped_t&;
		using const_reference_t = const stripped_t&;

		/**
		 * @brief
		 *  An argument type that can be used to pass variable of type T as a function
		 *  parameter without a copy step, and without the ability of the function to
		 *  modify the passed variable itself.  Non-constant pointers and references
		 *  can still be used to modify the objects they reference. This basically just
		 *  amounts to structured objects being passed by reference, with any const
		 *  modifier that might be present remaining.
		 *
		 * @code
		 * 	template<typename T>
		 * 	static void func(typename StdExt::Traits<T>::arg_non_copy_t arg)
		 * 	{
		 * 	}
		 * 	
		 * 	func<std::u8string>;         // static void func(const std::u8string&)
		 * 	func<std::u8string&>;        // static void func(std::u8string&)
		 * 	func<std::u8string&&>;       // static void func(std::u8string&&)
		 * 	func<std::u8string*>;        // static void func(std::u8string*)
		 * 	func<const std::u8string*>;  // static void func(const std::u8string*)
		 * 	func<int*>;                  // static void func(int*)
		 * 	func<int>;                   // static void func(int)
		 * @endcode
		 */
		using arg_non_copy_t = typename std::conditional_t<std::is_class_v<T>, const T&, T>;

		/**
		 * @brief
		 *  Provides a type to T that can be trivially be passed into a function without
		 *  calling a copy constructor, and protecting the parameter and what it references from
		 *  modification within the function.  For value types, this will simply be the value
		 *  itself.  For structured types and references, this will be constant reference.
		 *  For pointer types, this will be a constant pointer.
		 *
		 * @code
		 * 	template<typename T>
		 * 	static void func(typename StdExt::Traits<T>::arg_non_copy_t arg)
		 * 	{
		 * 	}
		 * 	
		 * 	func<std::u8string>;         // static void func(const std::u8string&)
		 * 	func<std::u8string&>;        // static void func(const std::u8string&)
		 * 	func<std::u8string&&>;       // static void func(const std::u8string&)
		 * 	func<std::u8string*>;        // static void func(const std::u8string*)
		 * 	func<const std::u8string*>;  // static void func(const std::u8string*)
		 * 	func<int*>;                  // static void func(const int*)
		 * 	func<int>;                   // static void func(int)
		 * @endcode
		 */
		using arg_non_copy_const_t =
			typename std::conditional_t<
				std::is_class_v<T> || std::is_reference_v<T>,
				const_reference_t,
				std::conditional_t<
					std::is_pointer_v<T>,
					std::add_const_t<T>,
					T
				>
			>;
	};

	template<typename ...args_t>
	struct Types;

	namespace detail
	{
		template<size_t N, typename ...args_t>
		struct first_types_n;

		template<typename ...args_t>
		struct first_types_n<0, args_t...>
		{
			using Type = Types<>;
		};

		template<typename arg_t, typename ...tail_args_t>
		struct first_types_n<1, arg_t, tail_args_t...>
		{
			using Type = Types<arg_t>;
		};

		template<size_t N, typename arg_t, typename ...args_tail_t>
		struct first_types_n<N, arg_t, args_tail_t...>
		{
			using Type = decltype(Types<arg_t>() + typename first_types_n<N - 1, args_tail_t...>::Type());
		};

		template<size_t N, typename ...args_t>
		struct last_types_n;

		template<typename ...args_t>
		struct last_types_n<0, args_t...>
		{
			using Type = Types<>;
		};

		template<size_t N, typename arg_0, typename ...args_t>
			requires ( N >= 1)
		struct last_types_n<N, arg_0, args_t...>
		{
			static_assert(N <= (sizeof...(args_t) + 1), "Not enough types for Types::last_n.");

			static constexpr auto type_obj()
				requires ( N == (sizeof...(args_t) + 1) )
			{
				return Types<arg_0, args_t...>();
			}

			static constexpr auto type_obj()
				requires ( N < (sizeof...(args_t) + 1) )
			{
				return typename last_types_n<N, args_t...>::Type();
			}

			using Type = decltype(type_obj());
		};
	}

	template<typename ...args_t>
	struct Types
	{
		/**
		 * @brief
		 *  Applies the types as template parameters to templ_t.
		 * 
		 * @code
		 *	Types<int, float>::apply<std::tuple>
		 */
		template<template<typename...> typename templ_t>
		using apply = templ_t<args_t...>;

		template<typename ...prefix_args_t>
		using prepend = Types<prefix_args_t..., args_t...>;

		template<typename ...postfix_args_t>
		using append = Types<args_t..., postfix_args_t...>;

		template<size_t index>
		using type_at = nth_type_t<index, args_t...>;

		static size_t count()
		{
			return sizeof...(args_t);
		}

		template<size_t N>
		using first_n = detail::first_types_n<N, args_t...>::Type;

		template<size_t N>
		using last_n = detail::last_types_n<N, args_t...>::Type;

		template<typename ...args_right_t>
		constexpr auto operator+(Types<args_right_t...> right)
		{
			return append<args_right_t...>();
		}

		constexpr bool operator==(const Types<args_t...>& other)
		{
			return true;
		}
	};
}

#endif // !_STD_EXT_TYPE_H_