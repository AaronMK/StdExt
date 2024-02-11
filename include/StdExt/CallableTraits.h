#ifndef _STD_EXT_FUNCTION_TRAITS_H_
#define _STD_EXT_FUNCTION_TRAITS_H_

#include "Concepts.h"

namespace StdExt
{
	template<typename T, typename ...args_t>
	struct CallableTraits;

	template<typename return_param_t, typename ...args_param_t>
	struct CallableTraits
	{
		using return_t = return_param_t;

		template <size_t index>
		using nth_arg_t = nth_type_t<index, args_param_t...>;

		static constexpr size_t arg_count = sizeof...(args_param_t);

		using args_t = Types<args_param_t...>;

		/**
		 * @brief
		 *  Appends the deduced <i>return_t</i>, and <i>args_t...</i> to a template definition
		 *  <i>tmp_t</i> with the associated <i>prefix_args...</i>.
		 * 
		 * @details
		 *  For example:
		 * 
		 * @code
		 *  template<typename t_a, typename t_b, typename ret_t, typename... args_t>
		 *  class ExampleFunctionClass
		 *  {
		 *  };
		 *
		 *  using func_t = std::function<void(std::string, int)>;
		 * 
		 *  class ExampleClass : public CallableTraits<func_t>::forward<ExampleFunctionClass, int, float>
		 *  {
		 *  };
		 *
		 *  // Same as:
		 *
		 *  class ExampleClass : public ExampleFunctionClass<int, float, void, std::string, int>
		 *  {
		 *  };
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward = tmp_t<prefix_args..., return_param_t, args_param_t...>;
		
		/**
		 * @brief
		 *  Appends the deduced <i>args_t...</i> to a template definition
		 *  <i>tmp_t</i> with the associated <i>prefix_args...</i>.
		 *
		 * @details
		 *  For example:
		 *
		 * @code
		 *  template<typename t_a, typename t_b, typename ret_t, typename... args_t>
		 *  class ExampleFunctionClass
		 *  {
		 *  };
		 *
		 * 	using func_t = std::function<void(std::string, int)>;
		 *
		 *  class ExampleClass : public CallableTraits<func_t>::forward<ExampleFunctionClass, int, float>
		 *  {
		 *  };
		 *
		 *  // Same as:
		 *
		 *  class ExampleClass : public ExampleFunctionClass<int, float, std::string, int>
		 *  {
		 *  };
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward_args = tmp_t<prefix_args..., args_param_t...>;
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

	/**
	 * @brief
	 *  Passes if T is callable with args_t and returns a type that is assignable to
	 *  ret_t.  If ret_t is void, it just checks that T is callable with args_t
	 *  regardless of what is returned.
	 */
	template<typename T, typename ret_t, typename ...args_t>
	concept CallableWith = 
		(
			NonVoid<ret_t> &&
			std::is_convertible_v<
				std::invoke_result_t<T, args_t...>, ret_t
			>
		) ||
		(
			std::is_same_v<void, ret_t> &&
			requires (T& func, args_t ...args)
			{
				{ func(std::forward<args_t>(args)...) };
			}
		);

	template<typename T, typename ret_t, typename ...args_t>
	concept HasSignature = 
		std::is_same_v<typename CallableTraits<T>::return_t, ret_t> &&
		std::is_same_v<
			typename CallableTraits<T>::args_t,
			typename CallableTraits<ret_t, args_t...>::args_t
		>;

	/**
	 * @brief
	 *  An invokable type that return ret_t when called with args_t.
	 */
	template<typename T, typename ret_t, typename ...args_t>
	concept ReturnsType = 
		std::is_same_v<std::invoke_result_t<T, args_t...>, ret_t>;

}

#endif // !_STD_EXT_FUNCTION_TRAITS_H_
