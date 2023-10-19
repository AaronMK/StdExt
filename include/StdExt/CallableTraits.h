#ifndef _STD_EXT_FUNCTION_TRAITS_H_
#define _STD_EXT_FUNCTION_TRAITS_H_

#include "Concepts.h"

namespace StdExt
{
	template<typename T, typename ...args_t>
	struct CallableTraits;

	template<typename return_param_t, typename ...args_t>
	struct CallableTraits
	{
		using return_t = return_param_t;

		template <size_t index>
		using nth_arg_t = nth_type_t<index, args_t...>;

		static constexpr size_t arg_count = sizeof...(args_t);

		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward = tmp_t<prefix_args..., return_param_t, args_t...>;
	};

	template<typename return_param_t, typename ...args_t>
	struct CallableTraits<return_param_t(*)(args_t...)>
		: CallableTraits<return_param_t, args_t...>
	{
	};

	template<Class class_t, typename return_param_t, typename ...args_t>
	struct CallableTraits<return_param_t(class_t::*)(args_t...)> :
		public CallableTraits<return_param_t, args_t...>
	{
	};

	template<Class class_t, typename return_param_t, typename ...args_t>
	struct CallableTraits<return_param_t(class_t::*)(args_t...) const> :
		public CallableTraits<return_param_t, args_t...>
	{
	};

	template<Class class_t>
	struct CallableTraits<class_t> :
		public CallableTraits<decltype(&class_t::operator()) >
	{
	};
}

#endif // !_STD_EXT_FUNCTION_TRAITS_H_
