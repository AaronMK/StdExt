#ifndef _STD_EXT_FUNCTION_TRAITS_H_
#define _STD_EXT_FUNCTION_TRAITS_H_

#include "Concepts.h"
#include "TemplateUtility.h"

#include <type_traits>

namespace StdExt
{
	namespace Detail
	{
		template<typename ...args_t>
		struct FunctionTraitsImpl;

		template<typename return_param_t, typename ...args_t>
		struct FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			static constexpr bool is_function_pointer = true;

			using return_type = return_param_t;
			using arg_types = Types<args_t...>;

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
		struct FunctionTraitsImpl<return_param_t(*)(args_t...) noexcept>
			: FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(*)(args_t...) noexcept;
			using target_type  = void;
			using class_type   = void;

			static constexpr bool is_member   = false;
			static constexpr bool is_const    = false;
			static constexpr bool is_noexcept = true;
		};

		template<typename return_param_t, typename ...args_t>
		struct FunctionTraitsImpl<return_param_t(*)(args_t...)>
			: FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(*)(args_t...);
			using target_type = void;
			using class_type  = void;

			static constexpr bool is_member   = false;
			static constexpr bool is_const    = false;
			static constexpr bool is_noexcept = false;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct FunctionTraitsImpl<return_param_t(class_t::*)(args_t...) noexcept> :
			public FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(class_t::*)(args_t...) noexcept;
			using target_type  = class_t*;
			using class_type   = class_t;

			static constexpr bool is_member   = true;
			static constexpr bool is_const    = false;
			static constexpr bool is_noexcept = true;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct FunctionTraitsImpl<return_param_t(class_t::*)(args_t...)> :
			public FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(class_t::*)(args_t...);
			using target_type  = class_t*;
			using class_type   = class_t;

			static constexpr bool is_member   = true;
			static constexpr bool is_const    = false;
			static constexpr bool is_noexcept = false;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct FunctionTraitsImpl<return_param_t(class_t::*)(args_t...) const noexcept> :
			public FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(class_t::*)(args_t...) const noexcept;
			using target_type  = const class_t*;
			using class_type   = class_t;

			static constexpr bool is_member   = true;
			static constexpr bool is_const    = true;
			static constexpr bool is_noexcept = true;
		};

		template<Class class_t, typename return_param_t, typename ...args_t>
		struct FunctionTraitsImpl<return_param_t(class_t::*)(args_t...) const> :
			public FunctionTraitsImpl<std::true_type, return_param_t, args_t...>
		{
			using raw_ptr_type = return_param_t(class_t::*)(args_t...) const;
			using target_type  = const class_t*;
			using class_type   = class_t;

			static constexpr bool is_member   = true;
			static constexpr bool is_const    = true;
			static constexpr bool is_noexcept = false;
		};

		template<typename T>
		struct FunctionTraitsImpl<T>
		{
			static constexpr bool is_function_pointer = false;
		};
	}

	/**
	 * @brief
	 *  Passes if the type is any type of raw function pointer.
	 */
	template<typename T>
	concept FunctionPointer = Detail::FunctionTraitsImpl<T>::is_function_pointer;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a static or free function.
	 */
	template<typename T>
	concept StaticFunctionPointer = FunctionPointer<T> && !Detail::FunctionTraitsImpl<T>::is_member;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a member function.
	 */
	template<typename T>
	concept MemberFunctionPointer = FunctionPointer<T> && Detail::FunctionTraitsImpl<T>::is_member;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a non-const member function.
	 */
	template<typename T>
	concept NonConstFunctionPointer = FunctionPointer<T> && Detail::FunctionTraitsImpl<T>::is_member && !Detail::FunctionTraitsImpl<T>::is_const;

	/**
	 * @brief
	 *  Passes if the type is a pointer to a const member function.
	 */
	template<typename T>
	concept ConstFunctionPointer = FunctionPointer<T> && Detail::FunctionTraitsImpl<T>::is_const;

	template<FunctionPointer ptr_t>
	class FunctionTraits
	{
	private:
		using traits_type = Detail::FunctionTraitsImpl<ptr_t>;

	public:

		/**
		 * @brief
		 *  The return type.
		 */
		using return_type = traits_type::return_type;

		/**
		 * @brief
		 *  The types of the arguments as a type definition Types
		 */
		using arg_types = traits_type::arg_types;

		/**
		 * @brief
		 *  The number of arguments 
		 */
		static constexpr size_t arg_count = traits_type::arg_count;

		/**
		 * @brief
		 *  True if the function is a non-static member function of a class or struct.
		 */
		static constexpr bool is_member = traits_type::is_member;

		/**
		 * @brief
		 *  True if the function is a constant member function.
		 */
		static constexpr bool is_const  = traits_type::is_const;

		/**
		 * @brief
		 *  True if the function does not throw exceptions.
		 */
		static constexpr bool is_noexcept  = traits_type::is_noexcept;

		template <size_t index>
		using nth_arg_t = traits_type::template nth_arg_t<index>;

		/**
		 * @brief
		 *  Appends the deduced <i>return_type</i>, and <i>args_t...</i> to a template definition
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
		 *	class ExampleClass : public ExampleFunctionClass<int, float, void, std::string, int>
		 *	{
		 *	};
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward = traits_type::template forward<tmp_t, prefix_args...>;
		
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
		using forward_args = traits_type::template forward_args<tmp_t, prefix_args...>;

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
		 *	using forward_foo = FunctionTraits<declspec(&foo)>::template apply_signature<std::function>;
		 *	using forward_func_a = FunctionTraits<declspec(&Bar::func_a)>::template apply_signature<std::function>;
		 *	using forward_func_b = FunctionTraits<declspec(&Bar::func_b)>::template apply_signature<std::function>;
		 * @endcode
		 */
		template<template<typename...> typename tmp_t>
		using apply_signature = traits_type::template apply_signature<tmp_t>;

		/**
		 * @brief 
		 *  If the function pointer is a non-static member function, this will be the type of which
		 *  it is a member.  Otherwise, the type will be _void_.
		 */
		using class_type = traits_type::class_type;

		using target_type = traits_type::target_type;

		using raw_ptr_type = traits_type::raw_ptr_type;

		const raw_ptr_type raw_pointer{nullptr};

		consteval FunctionTraits() = default;

		constexpr FunctionTraits(ptr_t func_ptr)
			: raw_pointer(func_ptr)
		{
		}
	};

	template<FunctionPointer ptr_t>
	FunctionTraits(ptr_t ptr) -> FunctionTraits<ptr_t>;

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
	class Function
	{
	private:
		using traits_type = Detail::FunctionTraitsImpl< decltype(func_ptr) >;

	public:

		/**
		 * @brief
		 *  The return type of the function.
		 */
		using return_type = traits_type::return_type;

		/**
		 * @brief
		 *  The types of the arguments as a specialization of Types.
		 */
		using arg_types  = traits_type::arg_types;

		/**
		 * @brief
		 *  The number of arguments the function takes.
		 */
		static constexpr size_t arg_count = traits_type::arg_count;

		/**
		 * @brief
		 *  True if the function is a non-static member function of a class or struct.  False if the
		 *  function is a static member function, or not a member function.
		 */
		static constexpr bool is_member = traits_type::is_member;

		/**
		 * @brief
		 *  True if the function is a constant member function.
		 */
		static constexpr bool is_const  = traits_type::is_const;

		/**
		 * @brief
		 *  True if the function does not throw exceptions.
		 */
		static constexpr bool is_noexcept  = traits_type::is_noexcept;

		/**
		 * @brief 
		 *  If the function pointer is a non-static member function, this will be the type of which
		 *  it is a member.  Otherwise, the type will be _void_.
		 */
		using class_type = traits_type::class_type;

		/**
		 * @brief 
		 *  If the function pointer is a non-static member function, this will the pointer type
		 *  of the target on which the function would be called, including a _const_ qualifier if
		 *  applicable.  Otherwise, the type will be _void_.
		 */
		using target_type = traits_type::target_type;

		/**
		 * @brief
		 *  The type of the function pointer.
		 */
		using raw_ptr_type = traits_type::raw_ptr_type;

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
		using nth_arg_t = traits_type::template nth_arg_t<index>;

		/**
		 * @brief
		 *  Appends the deduced <i>return_type</i>, and <i>args_t...</i> to a template definition
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
		 *	class ExampleClass : public ExampleFunctionClass<int, float, void, std::string, int>
		 *	{
		 *	};
		 * @endcode
		 */
		template<template<typename...> typename tmp_t, typename... prefix_args>
		using forward = traits_type::template forward<tmp_t, prefix_args...>;

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
		using apply_signature = traits_type::template apply_signature<tmp_t>;

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
		using forward_args = traits_type::template forward_args<tmp_t, prefix_args...>;
	};
}

#endif // _STD_EXT_FUNCTION_TRAITS_H_