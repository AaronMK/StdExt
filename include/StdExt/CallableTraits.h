#ifndef _STD_EXT_CALLABLE_TRAITS_H_
#define _STD_EXT_CALLABLE_TRAITS_H_

#include "Concepts.h"
#include "TemplateUtility.h"

#include <type_traits>

namespace StdExt
{
	/**
	 * @brief
	 *  Passes if the type is a class with a single call operator.
	 */
	template<typename T>
	concept CallableClass =
		Class<T> &&
		std::is_member_function_pointer_v<decltype(&T::operator())>;

	namespace Detail
	{
		template<typename ...args_t>
		struct CallableTraitsImpl;

		template<typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			static constexpr bool is_callable = true;

			using return_t = return_param_t;
			using params_t  = Types<args_t...>;

			static constexpr size_t arg_count = sizeof...(args_t);

			template <size_t index>
			using nth_arg_t = nth_type_t<index, args_t...>;

			template<template<typename...> typename tmp_t, typename... prefix_args>
			using forward = tmp_t<prefix_args..., return_param_t, args_t...>;
			
			template<template<typename...> typename tmp_t, typename... prefix_args>
			using forward_args = tmp_t<prefix_args..., args_t...>;
			
			template<template<typename...> typename tmp_t>
			using apply_signature = tmp_t<return_param_t(args_t...)>;
		};

		template<typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<return_param_t(*)(args_t...)>
			: CallableTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(*)(args_t...);
			using target_type = void;
			using class_type  = void;

			static constexpr bool is_member = false;
			static constexpr bool is_const  = true;

		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<return_param_t(class_t::*)(args_t...)> :
			public CallableTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(class_t::*)(args_t...);
			using target_type = class_t*;
			using class_type  = class_t;

			static constexpr bool is_member = true;
			static constexpr bool is_const  = false;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct CallableTraitsImpl<return_param_t(class_t::*)(args_t...) const> :
			public CallableTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(class_t::*)(args_t...) const;
			using target_type = const class_t*;
			using class_type  = class_t;

			static constexpr bool is_member = true;
			static constexpr bool is_const  = true;
		};

		template<>
		struct CallableTraitsImpl<std::false_type>
		{
			static constexpr bool is_callable = false;
		};

		template<CallableClass class_t>
		struct CallableTraitsImpl<class_t> :
			public CallableTraitsImpl<decltype(&class_t::operator())>
		{
		};

		template<typename T>
		struct CallableTraitsImpl<T> : CallableTraitsImpl<std::false_type>
		{
		};
	}

	/**
	 * @brief
	 *  Passes if the type is either a function pointer or a type with
	 *  a call operator.
	 */
	template<typename T>
	concept Callable = Detail::CallableTraitsImpl<T>::is_callable;

	/**
	 * @brief
	 *  Passes if the type is any type of raw function pointer.
	 */
	template<typename T>
	concept FunctionPointer = Callable<T> && !Class<T>;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a static or free function.
	 */
	template<typename T>
	concept StaticFunctionPointer = FunctionPointer<T> && !Detail::CallableTraitsImpl<T>::is_member;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a member function.
	 */
	template<typename T>
	concept MemberFunctionPointer = FunctionPointer<T> && Detail::CallableTraitsImpl<T>::is_member;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a non-const member function.
	 */
	template<typename T>
	concept NonConstFunctionPointer = FunctionPointer<T> && Detail::CallableTraitsImpl<T>::is_member && !Detail::CallableTraitsImpl<T>::is_const;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a const member function.
	 */
	template<typename T>
	concept ConstFunctionPointer = FunctionPointer<T> && Detail::CallableTraitsImpl<T>::is_member && Detail::CallableTraitsImpl<T>::is_const;

	/**
	 * @brief
	 *  Class to allow getting the traits of a function using a pointer to the function
	 *  directly as a non-type template parameter.
	 * 
	 * @tparam func_ptr
	 *  Pointer to the function
	 * 
	 * @code
	 * 
	 * @endcode
	 */
	template<FunctionPointer auto func_ptr>
	class FunctionTraits
	{
	private:
		using traits_t = Detail::CallableTraitsImpl< decltype(func_ptr) >;

	public:

		/**
		 * @brief
		 *  The return type of the function.
		 */
		using return_t = traits_t::return_t;

		/**
		 * @brief
		 *  The types of the arguments as a specialization of Types.
		 */
		using params_t  = traits_t::params_t;

		/**
		 * @brief
		 *  The number of arguments the function takes.
		 */
		static constexpr size_t arg_count = traits_t::arg_count;

		/**
		 * @brief
		 *  True if the function is a non-static member function of a class or struct.  False if the
		 *  function is a static member function, or not a member function.
		 */
		static constexpr bool is_member = traits_t::is_member;

		/**
		 * @brief
		 *  True if the callable is a constant member function or a non-member function.
		 */
		static constexpr bool is_const  = traits_t::is_const;

		/**
		 * @brief 
		 *  If the function pointer is a non-static member function, this will be the type of which
		 *  it is a member.  Otherwise, the type will be _void_.
		 */
		using class_type = traits_t::class_type;

		/**
		 * @brief 
		 *  If the function pointer is a non-static member function, this will the pointer type
		 *  of the target on which the function would be called, including a _const_ qualifier if
		 *  applicable.  Otherwise, the type will be _void_.
		 */
		using target_type = traits_t::target_type;

		/**
		 * @brief
		 *  The type of the function pointer.
		 */
		using raw_ptr_t = traits_t::raw_ptr_type;

		/**
		 * @brief
		 *  The raw function pointer passed as the template parameter.
		 */
		static constexpr auto raw_ptr = func_ptr;

		/**
		 * @brief
		 *  Gets the type of an argument of the function.
		 * 
		 * @tparam index
		 *  The zero based index of the argument on which to get the type.
		 */
		template <size_t index>
		using nth_arg_t = traits_t::template nth_arg_t<index>;

		/**
		 * @brief
		 *  Appends the deduced <i>return_t</i>, and <i>args_t...</i> to a template definition
		 *  <i>tmp_t</i> with the associated <i>prefix_args...</i>.
		 * 
		 * @details
		 *  For example:
		 * 
		 * @code
		 *	template<typename t_a, typename t_b, typename ret_t, typename... args_t>
		 *	class ExampleFunctionClass
		 *	{
		 *	};
		 *	
		 *	using func_t = std::function<void(std::string, int)>;
		 *	
		 *	class ExampleClass : public CallableTraits<func_t>::forward<ExampleFunctionClass, int, float>
		 *	{
		 *	};
		 *	
		 *	// Same as:
		 *	
		 *	class ExampleClass : public ExampleFunctionClass<int, float, void, std::string, int>
		 *	{
		 *	};
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward = traits_t::template forward<tmp_t, prefix_args...>;

		/**
		 * @brief
		 *  Applies the function signature as a template parameter to the given template
		 *  in a similar format to that of std::function.
		 * 
		 * @code
		 *	static int foo(const std::string& param_a, int param_b)
		 *	{
		 *		return 1;
		 *	}
		 *
		 *	class BarNonCost
		 *	{
		 *	public:
		 *		int func_a(const std::string& param_a, int param_b);
		 *		int func_b(const std::string& param_a, int param_b) const;
		 *	};
		 *
		 *	// All of these would be std::function<int(const std::string&, int)>
		 *	using forward_foo = FunctionTraits<&foo>::template apply_signature<std::function>;
		 *	using forward_func_a = FunctionTraits<&Bar::func_a>::template apply_signature<std::function>;
		 *	using forward_func_b = FunctionTraits<&Bar::func_b>::template apply_signature<std::function>;
		 * @endcode
		 */
		template<template<typename...> typename tmp_t>
		using apply_signature = traits_t::template apply_signature<tmp_t>;

		/**
		 * @brief
		 *  Appends the deduced <i>args_t...</i> to a template definition
		 *  <i>tmp_t</i> with the associated <i>prefix_args...</i>.
		 * 
		 * @details
		 *  For example:
		 * 
		 * @code
		 *	template<typename t_a, typename t_b, typename ret_t, typename... args_t>
		 *	class ExampleFunctionClass
		 *	{
		 *	};
		 *
		 *	using func_t = std::function<void(std::string, int)>;
		 *
		 *	class ExampleClass : public FunctionTraits<func_t>::forward<ExampleFunctionClass, int, float>
		 *	{
		 *	};
		 *
		 *	// Same as:
		 *
		 *	class ExampleClass : public ExampleFunctionClass<int, float, std::string, int>
		 *	{
		 *	};
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward_args = traits_t::template forward_args<tmp_t, prefix_args...>;
	};

	template<Callable callable_t>
	class CallableTraits
	{
	private:
		using traits_t = Detail::CallableTraitsImpl<callable_t>;

	public:

		/**
		 * @brief
		 *  The return type.
		 */
		using return_t = traits_t::return_t;

		/**
		 * @brief
		 *  The types of the arguments as a type definition Types
		 */
		using params_t  = traits_t::params_t;

		/**
		 * @brief
		 *  The number of arguments 
		 */
		static constexpr size_t arg_count = traits_t::arg_count;

		template <size_t index>
		using nth_arg_t = traits_t::template nth_arg_t<index>;

		/**
		 * @brief
		 *  Appends the deduced <i>return_t</i>, and <i>args_t...</i> to a template definition
		 *  <i>tmp_t</i> with the associated <i>prefix_args...</i>.
		 * 
		 * @details
		 *  For example:
		 * 
		 * @code
		 *	template<typename t_a, typename t_b, typename ret_t, typename... args_t>
		 *	class ExampleFunctionClass
		 *	{
		 *	};
		 *	
		 *	using func_t = std::function<void(std::string, int)>;
		 *	
		 *	class ExampleClass : public CallableTraits<func_t>::forward<ExampleFunctionClass, int, float>
		 *	{
		 *	};
		 *	
		 *	// Same as:
		 *	
		 *	class ExampleClass : public ExampleFunctionClass<int, float, void, std::string, int>
		 *	{
		 *	};
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward = traits_t::template forward<tmp_t, prefix_args...>;

		/**
		 * @brief
		 *  Applies the function signature as a template parameter to the given template
		 *  in a similar format to that of std::function.
		 * 
		 * @code
		 *	static int foo(const std::string& param_a, int param_b)
		 *	{
		 *		return 1;
		 *	}
		 *	
		 *	class Bar
		 *	{
		 *	public:
		 *		int func_a(const std::string& param_a, int param_b);
		 *		int func_b(const std::string& param_a, int param_b) const;
		 *	};
		 *	 
		 *	// All of these would be std::function<int(const std::string&, int)>
		 *	using forward_foo = CallableTraits<declspec(&foo)>::template apply_signature<std::function>;
		 *	using forward_func_a = CallableTraits<declspec(&Bar::func_a)>::template apply_signature<std::function>;
		 *	using forward_func_b = CallableTraits<declspec(&Bar::func_b)>::template apply_signature<std::function>;
		 * @endcode
		 */
		template<template<typename...> typename tmp_t>
		using apply_signature = traits_t::template apply_signature<tmp_t>;
		
		/**
		 * @brief
		 *  Appends the deduced <i>args_t...</i> to a template definition
		 *  <i>tmp_t</i> with the associated <i>prefix_args...</i>.
		 * 
		 * @details
		 *  For example:
		 * 
		 * @code
		 *	template<typename t_a, typename t_b, typename ret_t, typename... args_t>
		 *	class ExampleFunctionClass
		 *	{
		 *	};
		 *		
		 *		using func_t = std::function<void(std::string, int)>;
		 *	
		 *	class ExampleClass : public CallableTraits<func_t>::forward<ExampleFunctionClass, int, float>
		 *	{
		 *	};
		 *	
		 *	// Same as:
		 *	
		 *	class ExampleClass : public ExampleFunctionClass<int, float, std::string, int>
		 *	{
		 *	};
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward_args = traits_t::template forward_args<tmp_t, prefix_args...>;

		/**
		 * @brief
		 *  True if the function is a non-static member function of a class or struct.
		 */
		static constexpr bool is_member = traits_t::is_member;

		/**
		 * @brief
		 *  True if the callable is a constant member function or a non-member function.
		 */
		static constexpr bool is_const  = traits_t::is_const;

		/**
		 * @brief 
		 *  If the function pointer is a non-static member function, this will be the type of which
		 *  it is a member.  Otherwise, the type will be _void_.
		 */
		using class_type = traits_t::class_type;

		using target_type = traits_t::target_type;

		const traits_t::raw_ptr_type raw_call_ptr;

		template<FunctionPointer ptr_t>
		consteval CallableTraits(ptr_t callable)
			: raw_call_ptr(callable)
		{
		}

		template<CallableClass class_t>
		constexpr CallableTraits(const class_t& callable)
			: raw_call_ptr(&class_t::operator())
		{
		}
	};

	template<FunctionPointer ptr_t>
	CallableTraits(ptr_t ptr) -> CallableTraits<ptr_t>;

	template<CallableClass T>
	CallableTraits(const T& obj) -> CallableTraits<T>;
}

#endif // !_STD_EXT_CALLABLE_TRAITS_H_
