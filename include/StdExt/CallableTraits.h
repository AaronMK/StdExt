#ifndef _STD_EXT_FUNCTION_TRAITS_H_
#define _STD_EXT_FUNCTION_TRAITS_H_

#include "Concepts.h"

namespace StdExt
{
	namespace details
	{
		template<typename T, typename ...args_t>
		struct CallableTraitsImpl;

		template<typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl
		{
			using return_t = return_param_t;

			template <size_t index>
			using nth_arg_t = nth_type_t<index, args_t...>;

			static constexpr size_t arg_count = sizeof...(args_t);

			template<template<typename...> typename tmp_t, typename... prefix_args>
			using forward = tmp_t<prefix_args..., return_param_t, args_t...>;
			
			template<template<typename...> typename tmp_t, typename... prefix_args>
			using forward_args = tmp_t<prefix_args..., args_t...>;
		};

		template<typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<return_param_t(*)(args_t...)>
			: CallableTraitsImpl<return_param_t, args_t...>
		{
			using target_type = void;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<return_param_t(class_t::*)(args_t...)> :
			public CallableTraitsImpl<return_param_t, args_t...>
		{
			using target_type = class_t*;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<return_param_t(class_t::*)(args_t...) const> :
			public CallableTraitsImpl<return_param_t, args_t...>
		{
			using target_type = const class_t*;
		};

		template<Class class_t>
		struct CallableTraitsImpl<class_t> :
			public CallableTraitsImpl<decltype(&class_t::operator()) >
		{
		};
	}

	template<typename callable_t>
	class CallableTraits
	{
	private:
		using traits_t = details::CallableTraitsImpl<callable_t>;

	public:

		using return_t = traits_t::return_t;

		template <size_t index>
		using nth_arg_t = traits_t::template nth_arg_t<index>;

		static constexpr size_t arg_count = traits_t::arg_count;

		using target_type = traits_t::target_type;

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
		using forward = traits_t::template forward<tmp_t, prefix_args...>;
		
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
		using forward_args = traits_t::template forward_args<tmp_t, prefix_args...>;
	};
}

#endif // !_STD_EXT_FUNCTION_TRAITS_H_
