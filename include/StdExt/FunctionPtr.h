#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include "Concepts.h"
#include "CallableTraits.h"
#include "TemplateUtility.h"
#include "Type.h"

#include "Memory/Casting.h"

#include <functional>
#include <utility>

namespace StdExt
{

	template<typename... types_t>
	class FunctionPtr;

	template<typename return_t, typename... args_t>
	class FunctionPtr<return_t(args_t...)>;

	template<typename T>
	class FunctionPtrTraits;

	template<Callable ptr_t>
	class FunctionPtrTraits<ptr_t> : public CallableTraits<ptr_t>
	{
	private:
		using base_t = CallableTraits<ptr_t>;

	public:
		consteval FunctionPtrTraits(ptr_t func_ptr)
			: base_t(func_ptr)
		{
		}

		using function_ptr_t = base_t::template apply_signiture<FunctionPtr>;
	};

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
			using ptr_type = decltype(func_ptr);
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