#ifndef _STD_EXT_TEMPLATE_TRAITS_H_
#define _STD_EXT_TEMPLATE_TRAITS_H_

#include "Type.h"

namespace StdExt
{
	template<typename ...args_t>
	struct Types;

	namespace details
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
		 *	Types<int, float>::apply<std::tuple>  // same as std::tuple<int, float>
		 * @endcode
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
		using first_n = details::first_types_n<N, args_t...>::Type;

		template<size_t N>
		using last_n = details::last_types_n<N, args_t...>::Type;

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

#endif // !_STD_EXT_TEMPLATE_TRAITS_H_
