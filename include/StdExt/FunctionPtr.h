#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include "CallableTraits.h"

#include "Memory/Casting.h"

#include <concepts>
#include <type_traits>

namespace StdExt
{
	template<typename... types_t>
	class Delegate;

	/**
	 * @brief
	 *  Type-erased wrapping of a function pointer and, if applicable, a target object.  This is suitable
	 *  for consteval contexts.
	 * 
	 * @tparam return_t 
	 * @tparam ...args_t 
	 */
	template<typename return_t, typename... args_t>
	class Delegate<return_t(args_t...)>
	{
	public:
		using func_ptr_t = return_t(*)(void*, args_t&&...);
		
		const func_ptr_t jump_func{nullptr};
		const void* target{nullptr};

		consteval Delegate() = default;

		constexpr return_t operator()(args_t... args) const
		{
			return std::invoke(jump_func, target, std::forward<args_t>(args)...);
		}

		constexpr Delegate(func_ptr_t _jump_func, void* _target)
			: jump_func(_jump_func),
			  target(_target)
		{
		}
	};

	template<typename... types>
	class StaticFunctionPtr;

	template<typename return_t, typename... args_t>
	class StaticFunctionPtr<return_t(args_t...)>
	{
	public:
		using raw_ptr_t = return_t(*)(args_t...);
		const raw_ptr_t raw_ptr;

		consteval StaticFunctionPtr(return_t(*func_ptr)(args_t...))
			: raw_ptr(func_ptr)
		{
		}
	};

	template<typename... types>
	class MemberFunctionPtr;

	template<Class class_t, typename return_t, typename... args_t>
	class MemberFunctionPtr<class_t, return_t(args_t...)>
	{
	public:
		using raw_ptr_t = return_t(class_t::*)(args_t...);
		const raw_ptr_t raw_ptr;

		consteval MemberFunctionPtr(return_t(class_t::*func_ptr)(args_t...))
			: raw_ptr(func_ptr)
		{
		}
	};

	template<typename... types>
	class ConstMemberFunctionPtr;

	template<Class class_t, typename return_t, typename... args_t>
	class ConstMemberFunctionPtr<class_t, return_t(args_t...)>
	{
	public:
		using raw_ptr_t = return_t(class_t::*)(args_t...);
		const raw_ptr_t raw_ptr;

		consteval ConstMemberFunctionPtr(return_t(class_t::*func_ptr)(args_t...))
			: raw_ptr(func_ptr)
		{
		}
	};

	template<typename first_t, typename... types>
	class getOverload;

	template<typename return_t, typename... args_t>
	class getOverload<return_t(args_t...)>
	{
	public:
		template<Class class_t>
		static consteval auto try_cast(return_t(class_t::*func_ptr)(args_t...))
		{
			return func_ptr;
		}
		
		template<Class class_t>
		static consteval auto try_cast(return_t(class_t::*func_ptr)(args_t...) const)
		{
			return func_ptr;
		}

		static consteval auto try_cast(return_t(* func_ptr)(args_t...))
		{
			return func_ptr;
		}
	};

	namespace Detail
	{
		template<bool is_const, typename class_t, typename return_t, typename... args_t>
		consteval auto makeFunctionPtr()
		{
			if constexpr ( std::same_as<class_t, void> )
			{
				return static_cast<return_t(*)(args_t...)>(nullptr);
			}
		};
	}

	template<typename... types_t>
	class FunctionPtr;

	template<typename return_t, typename... args_t>
	class FunctionPtr<return_t(args_t...)>
	{
	private:
		using func_ptr_t = return_t(*)(void*, args_t&&...);

		func_ptr_t mStaticFunc{nullptr};
		void*      mTarget{nullptr};

	public:
		template<FunctionPointer auto func_ptr>
		class RawFunctionParam : public CallableTraits<decltype(func_ptr)>
		{
		public:
			using ptr_type        = decltype(func_ptr);
			using ptr_traits      = CallableTraits<ptr_type>;
			using ptr_target_type = ptr_traits::target_type;
			using ptr_return_type = ptr_traits::return_t;

			static constexpr bool ptr_is_member = ptr_traits::is_member;
			
			static_assert(
				( ptr_is_member && std::invocable<ptr_type, ptr_target_type, args_t...> ) ||
				( !ptr_is_member && std::invocable<ptr_type, args_t...> ),
				"Raw function pointer must be invocable with argument types of the FunctionPtr to which it "
				"is being bound."
			);

			static_assert(
				std::convertible_to<ptr_return_type, return_t>,
				"Raw function return type must be convertable to FuncionPtr return type."
			);

			static return_t jump_func(void* target, args_t&&... args)
			{
				if constexpr ( ptr_is_member )
				{
					ptr_target_type casted_target = access_as<ptr_target_type>(target);
					return std::invoke(func_ptr, casted_target, std::forward<args_t>(args)...);
				}
				else
				{
					return std::invoke(func_ptr, std::forward<args_t>(args)...);
				}
			}
		};

		consteval FunctionPtr() = default;

		constexpr FunctionPtr(const FunctionPtr& other) = default;
		
		constexpr FunctionPtr(FunctionPtr&& other)
			: mStaticFunc(other.mStaticFunc),
			  mTarget(other.mTarget)
		{
			other.mStaticFunc = nullptr;
			other.mTarget = nullptr;
		}

		constexpr FunctionPtr& operator=(const FunctionPtr& other) = default;

		constexpr FunctionPtr operator=(FunctionPtr&& other)
		{
			mStaticFunc = other.mStaticFunc;
			mTarget = other.mTarget;

			other.mStaticFunc = nullptr;
			other.mTarget = nullptr;

			return *this;
		}

		template<FunctionPointer auto func_ptr>
			requires RawFunctionParam<func_ptr>::is_member
		void bind(RawFunctionParam<func_ptr>::target_type target)
		{
			mStaticFunc = &RawFunctionParam<func_ptr>::jump_func;
			mTarget     = access_as<void*>(target);
		}

		template<FunctionPointer auto func_ptr>
			requires (false == RawFunctionParam<func_ptr>::is_member)
		void bind()
		{
			mStaticFunc = &RawFunctionParam<func_ptr>::jump_func;
			mTarget     = nullptr;
		}

		constexpr return_t operator()(args_t&&... args)
		{
			return std::invoke(mStaticFunc, mTarget, std::forward<args_t>(args)...);
		}

		constexpr operator bool() const
		{
			return (nullptr == mStaticFunc);
		}

		constexpr bool operator==(const FunctionPtr& other)
		{
			return (other.mStaticFunc == mStaticFunc && other.mTarget == mTarget);
		}
	};
}

#endif //!_STD_EXT_FUNCTION_PTR_H_