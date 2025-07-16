#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include "FunctionTraits.h"

#include "Memory/Casting.h"

#include <concepts>
#include <type_traits>

namespace StdExt
{
	namespace Detail
	{
		template<FunctionPointer ptr_t>
		class BoundFunctionPointer
		{
		public:
			using traits_t      = FunctionTraits<ptr_t>;
			using target_type   = std::conditional_t<traits_t::is_member, typename traits_t::target_type, void*>;
			using function_type = traits_t::raw_ptr_t;
			using return_type   = traits_t::return_type;

			struct MemberParams
			{
				function_type mFunction{nullptr};
				target_type   mTarget{nullptr};
			};

			using PtrParam = std::conditional_t<traits_t::is_member, MemberParams, function_type>;
			PtrParam mPtrParam;

			consteval BoundFunctionPointer() = default;

			constexpr BoundFunctionPointer(const BoundFunctionPointer&) = default;
			BoundFunctionPointer(BoundFunctionPointer&& other)
			{
				mPtrParam       = other.mPtrParam;
				other.mPtrParam = PtrParam{};
			}

			constexpr BoundFunctionPointer(function_type func_ptr)
				requires (!traits_t::is_member)
				: mPtrParam(func_ptr)
			{
			}

			constexpr BoundFunctionPointer(function_type func_ptr, target_type target)
				requires (traits_t::is_member)
				: mPtrParam{.mFunction = func_ptr, .mTarget = target}
			{
			}

			BoundFunctionPointer& operator=(const BoundFunctionPointer&) = default;
			BoundFunctionPointer& operator=(BoundFunctionPointer&& other)
			{
				mPtrParam       = other.mPtrParam;
				other.mPtrParam = PtrParam{};

				return *this;
			}

			constexpr bool operator==(const BoundFunctionPointer&) const  = default;
			constexpr auto operator<=>(const BoundFunctionPointer&) const = default;

			template<typename... args_t>
			constexpr return_type operator()(args_t&&... args) const noexcept(traits_t::is_noexcept)
				requires (!traits_t::is_member)
			{
				static_assert( std::invocable<function_type, args_t...> );
				return std::invoke(mPtrParam, std::forward<args_t>(args)...);
			}

			template<typename... args_t>
			constexpr return_type operator()(args_t&&... args) const noexcept(traits_t::is_noexcept)
				requires (traits_t::is_member)
			{
				static_assert( std::invocable<function_type, target_type, args_t...> );
				return std::invoke(mPtrParam.mFunction, mPtrParam.mTarget, std::forward<args_t>(args)...);
			}
		};

		template<MemberFunctionPointer ptr_t>
		BoundFunctionPointer(ptr_t ptr, typename FunctionTraits<ptr_t>::target_type traget) -> BoundFunctionPointer<ptr_t>;

		template<StaticFunctionPointer ptr_t>
		BoundFunctionPointer(ptr_t ptr) -> BoundFunctionPointer<ptr_t>;

		template<FunctionPointer auto ptr_t>
		class BoundFunction;

		template<MemberFunctionPointer auto mem_func>
		class BoundFunction<mem_func>
		{
		public:
			using traits   = Function<mem_func>;
			using return_type = traits::return_type;
			using target_t = traits::target_type;

			target_t mTarget{nullptr};

			constexpr BoundFunction(target_t target)
				: mTarget(target)
			{
			}

			template<typename... args_t>
			constexpr return_type operator()(args_t&&... args) const noexcept(traits::is_noexcept)
			{
				static_assert(std::invocable<decltype(mem_func), target_t, args_t...>);
				return std::invoke(mem_func, mTarget, std::forward<args_t>(args)...);
			}
		};

		template<StaticFunctionPointer auto static_func>
		struct BoundFunction<static_func>
		{
			using traits               = Function<static_func>;
			using return_type             = traits::return_type;

			template<typename... args_t>
			constexpr return_type operator()(args_t&&... args) const noexcept(traits::is_noexcept)
			{
				static_assert(std::invocable<decltype(static_func), args_t...>);
				return std::invoke(static_func, std::forward<args_t>(args)...);
			}
		};

		template<typename return_type, typename... args_t>
		struct ObjectJump
		{
			template<bool is_const, CallableWith<return_type, args_t...> callable_t>
			static return_type jump_func(void* target, args_t&&... args)
			{
				if constexpr (is_const)
				{
					const callable_t* typed_ptr = access_as<const callable_t*>(target);
					return (*typed_ptr)(std::forward<args_t>(args)...);
				}
				else
				{
					callable_t* typed_ptr = access_as<callable_t*>(target);
					return (*typed_ptr)(std::forward<args_t>(args)...);
				}
			}
		};
	}

	template<StaticFunctionPointer auto func_ptr>
	static constexpr auto bind()
	{
		return Detail::BoundFunction<func_ptr>();
	}

	template<MemberFunctionPointer auto func_ptr>
	static constexpr auto bind(typename Function<func_ptr>::target_type target)
	{
		return Detail::BoundFunction<func_ptr>(target);
	}

	template<StaticFunctionPointer func_ptr_t>
	static constexpr auto bind(func_ptr_t ptr)
	{
		return Detail::BoundFunctionPointer(ptr);
	}

	template<MemberFunctionPointer func_ptr_t>
	static constexpr auto bind(func_ptr_t ptr, typename FunctionTraits<func_ptr_t>::target_type target)
	{
		return Detail::BoundFunctionPointer(ptr, target);
	}

	template<typename... types_t>
	class CallablePtr;

	template<typename return_type, typename... args_t>
	class CallablePtr<return_type(args_t...)>
	{
	private:
		template<bool is_const, CallableWith<return_type, args_t...> callable_t>
		static return_type jump_func(void* target, args_t&&... args)
		{
			if constexpr (std::is_void_v<return_type>)
			{
				if constexpr (is_const)
				{
					const callable_t* typed_ptr = access_as<const callable_t*>(target);
					(*typed_ptr)(std::forward<args_t>(args)...);
				}
				else
				{
					callable_t* typed_ptr = access_as<callable_t*>(target);
					(*typed_ptr)(std::forward<args_t>(args)...);
				}
			}
			else
			{
				if constexpr (is_const)
				{
					const callable_t* typed_ptr = access_as<const callable_t*>(target);
					return (*typed_ptr)(std::forward<args_t>(args)...);
				}
				else
				{
					callable_t* typed_ptr = access_as<callable_t*>(target);
					return (*typed_ptr)(std::forward<args_t>(args)...);
				}
			}
		}

		using jup_func_t = return_type(*)(void*, args_t&&...);

		void* mObj{};
		jup_func_t mCaller{};

	public:
		consteval CallablePtr() = default;

		constexpr CallablePtr(const CallablePtr&) = default;

		constexpr CallablePtr(CallablePtr&& other)
		{
			mObj = other.mObj;
			other.mObj = nullptr;
			
			mCaller = other.mCaller;
			other.mCaller = nullptr;
		}

		template<CallableWith<return_type, args_t...> callable_t>
		constexpr CallablePtr(callable_t* callable_obj)
		{
			using core_t = Type<decltype(callable_obj)>::core;
			constexpr bool is_const = ConstType<decltype(callable_obj)>;

			mObj = access_as<void*>(callable_obj);
			mCaller = &jump_func<is_const, core_t>;
		}

		constexpr CallablePtr& operator=(const CallablePtr&) = default;

		constexpr CallablePtr&  operator=(CallablePtr&& other)
		{
			mObj = other.mObj;
			other.mObj = nullptr;
			
			mCaller = other.mCaller;
			other.mCaller = nullptr;

			return *this;
		}

		constexpr return_type operator()(args_t... args) const
		{
			return std::invoke(mCaller, mObj, std::forward<args_t>(args)...);
		}

		constexpr operator bool() const
		{
			return (nullptr != mCaller);
		}
		
		constexpr bool operator==(const CallablePtr&) const  = default;
		constexpr auto operator<=>(const CallablePtr&) const = default;
	};
}

#endif //!_STD_EXT_FUNCTION_PTR_H_