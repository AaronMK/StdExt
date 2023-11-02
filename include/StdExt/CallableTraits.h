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
		 * 	using func_t = std::function<void(std::string, int)>;
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
		using forward = tmp_t<prefix_args..., return_param_t, args_t...>;
		
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
		using forward_args = tmp_t<prefix_args..., args_t...>;
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
