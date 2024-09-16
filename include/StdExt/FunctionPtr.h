#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include "Concepts.h"
#include "TemplateUtility.h"

#include <functional>
#include <utility>

namespace StdExt
{
	/**
	 * @brief
	 *  Convenience definition of a static function pointer to avoid syntax
	 *  that can be messy and hard to remember.
	 *
	 * @tparam return_t
	 *  return_t The return type of the function.
	 *
	 * @tparam args_t
	 *  The types of the arguments.
	 */
	template<typename return_t, typename... args_t>
	class StaticFunctionPtr
	{
	public:
		using raw_ptr_t = return_t(*)(args_t...);

		consteval StaticFunctionPtr(return_t(*func_ptr)(args_t...))
			: mPtr(func_ptr)
		{
		}

		constexpr StaticFunctionPtr(const StaticFunctionPtr& other) = default;
		constexpr StaticFunctionPtr& operator=(const StaticFunctionPtr& other) = default;

		constexpr inline return_t operator()(args_t... args) const
		{
			return std::invoke(mPtr, std::forward<args_t>(args)...);
		}

	public:
		raw_ptr_t mPtr;
	};

	/**
	 * @brief
	 *  Convenience definition of a member function pointer to avoid syntax that can
	 *  be messy and hard to remember.
	 *
	 * @tparam class_t
	 *  The class of the member function.
	 *
	 * @tparam return_t
	 *  return_t The return type of the function.
	 *
	 * @tparam args_t
	 *  The types of the arguments.
	 */
	template<Class class_t, typename return_t, typename... args_t>
	class MemberFunctionPtr
	{
	public:
		using raw_ptr_t = return_t(class_t::*)(args_t...);

		consteval MemberFunctionPtr(return_t(class_t::*func_ptr)(args_t...))
			: mPtr(func_ptr)
		{
		}

		constexpr MemberFunctionPtr(const MemberFunctionPtr& other) = default;
		constexpr MemberFunctionPtr& operator=(const MemberFunctionPtr& other) = default;

		constexpr inline return_t operator()(class_t* target, args_t... args) const
		{
			return std::invoke(mPtr, target, std::forward<args_t>(args)...);
		}

	public:
		raw_ptr_t mPtr;
	};

	/**
	 * @brief
	 *  Convenience definition of a const member function pointer to avoid syntax
	 *  that can be messy and hard to remember.
	 *
	 * @tparam class_t
	 *  The class of the member function.
	 *
	 * @tparam return_t
	 *  return_t The return type of the function.
	 *
	 * @tparam args_t
	 *  The types of the arguments.
	 */
	template<Class class_t, typename return_t, typename... args_t>
	class ConstMemberFunctionPtr
	{
	public:
		using raw_ptr_t = return_t(class_t::*)(args_t...) const;

		consteval ConstMemberFunctionPtr(return_t(class_t::*func_ptr)(args_t...) const)
			: mPtr(func_ptr)
		{
		}

		constexpr ConstMemberFunctionPtr(const ConstMemberFunctionPtr& other) = default;
		constexpr ConstMemberFunctionPtr& operator=(const ConstMemberFunctionPtr& other) = default;

		constexpr inline return_t operator()(const class_t* target, args_t... args) const
		{
			return std::invoke(mPtr, target, std::forward<args_t>(args)...);
		}

	public:
		raw_ptr_t mPtr;
	};
#if 0
	namespace details
	{
		template<typename T>
		class FuncPtrTypeTraits
		{
		public:
			static constexpr bool is_function_pointer = false;
		};

		template<typename return_param_t, typename ...args_t>
		class FuncPtrTypeTraits<return_param_t(*)(args_t...)>
		{
		public:
			static constexpr bool is_function_pointer = true;

			static constexpr bool is_static           = true;
			static constexpr bool is_non_const_member = false;
			static constexpr bool is_const_member     = false;
			static constexpr bool is_member           = false;

			using class_type     = void;
			using return_type    = return_param_t;
			using arg_types      = typename Types<args_t...>;
			using void_cast_type = void*;
			using raw_ptr_type   = return_param_t(*)(args_t...);
			using func_ptr_type  = FunctionPtr<return_param_t, args_t...>;
		};

		template<auto func_ptr>
		class FuncPtrTraitsImpl : public FuncPtrTypeTraits<decltype(func_ptr)>
		{
		};

	}
#endif
}

#endif //!_STD_EXT_FUNCTION_PTR_H_