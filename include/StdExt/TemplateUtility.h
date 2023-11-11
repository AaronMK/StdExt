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
		struct first_n;

		template<typename arg_t>
		struct first_n<1, arg_t>
		{
			using Type = Types<arg_t>;
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
		 *	Types<int, float>::apply<std::tuple
		 */
		template<template<typename...> typename templ_t>
		using apply = templ_t<args_t...>;

		template<typename ...prefix_args_t>
		using prepend = Types<prefix_args_t..., args_t...>;

		template<typename ...postfix_args_t>
		using append = Types<args_t..., postfix_args_t...>;

		template<size_t index>
		using type_at = nth_type_t<index, args_t...>;

		static constexpr size_t count = sizeof...(args_t);

		template<typename ...args_right_t>
		constexpr auto operator+(Types<args_right_t...> right)
		{
			return Types<args_t...>::append<args_right_t...>();
		}

		constexpr bool operator==(const Types<args_t...>& other)
		{
			return true;
		}
	};
}

#endif // !_STD_EXT_TEMPLATE_TRAITS_H_