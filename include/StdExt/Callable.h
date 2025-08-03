#ifndef _STD_EXT_CALLABLE_H_
#define _STD_EXT_CALLABLE_H_

#include "FunctionTraits.h"

#include "Memory/Casting.h"

#include <compare>
#include <concepts>
#include <type_traits>

namespace StdExt
{
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

		template<MemberFunctionPointer auto func>
		static return_type member_jump(void* target, args_t&&... args)
		{
			using target_type = Function<func>::target_type;
			
			target_type typed_target = access_as<target_type>(target);

			if constexpr (std::is_void_v<return_type>)
				std::invoke(func, typed_target, std::forward<args_t>(args)...);
			else
				return std::invoke(func, typed_target, std::forward<args_t>(args)...);
		}

		template<StaticFunctionPointer auto func>
		static return_type static_jump(void* target, args_t&&... args)
		{
			if constexpr (std::is_void_v<return_type>)
				std::invoke(func, std::forward<args_t>(args)...);
			else
				return std::invoke(func, std::forward<args_t>(args)...);
		}

		using jup_func_t = return_type(*)(void*, args_t&&...);

		void* mObj{nullptr};
		jup_func_t mCaller{nullptr};

	public:
		using my_type = CallablePtr<return_type(args_t...)>;
		
		constexpr CallablePtr() = default;

		constexpr CallablePtr(const CallablePtr&) = default;

		constexpr CallablePtr(CallablePtr&& other) noexcept
		{
			mObj       = other.mObj;
			other.mObj = nullptr;

			mCaller       = other.mCaller;
			other.mCaller = nullptr;
		}

		template<CallableWith<return_type, args_t...> callable_t>
		constexpr CallablePtr(callable_t* callable_obj)
		{
			using core_t = Type<decltype(callable_obj)>::core;
			constexpr bool is_const = ConstType<decltype(callable_obj)>;

			mObj    = access_as<void*>(callable_obj);
			mCaller = &jump_func<is_const, core_t>;
		}

		template<MemberFunctionPointer auto func>
		void bind(Function<func>::target_type target)
		{
			static_assert(
				std::invocable<decltype(func), typename Function<func>::target_type, args_t...>,
				"func must be invokable with CallablePtr argument types."
			);

			mObj    = access_as<void*>(target);
			mCaller = &member_jump<func>;
		}

		template<StaticFunctionPointer auto func>
		constexpr void bind()
		{
			static_assert(
				std::invocable<decltype(func), args_t...>,
				"func must be invokable with CallablePtr argument types."
			);

			mObj    = nullptr;
			mCaller = &static_jump<func>;
		}

		void clear()
		{
			mObj    = nullptr;
			mCaller = nullptr;
		}

		constexpr CallablePtr& operator=(const CallablePtr&) = default;

		constexpr CallablePtr&  operator=(CallablePtr&& other)
		{
			mObj       = other.mObj;
			other.mObj = nullptr;
			
			mCaller       = other.mCaller;
			other.mCaller = nullptr;

			return *this;
		}

		constexpr return_type operator()(args_t... args) const
		{
		
			if constexpr (std::is_void_v<return_type>)
				std::invoke(mCaller, mObj, std::forward<args_t>(args)...);
			else
				return std::invoke(mCaller, mObj, std::forward<args_t>(args)...);
		}

		constexpr operator bool() const
		{
			return (nullptr != mCaller);
		}

		constexpr bool hasPointer() const
		{
			return (nullptr != mCaller);
		}
		
		constexpr bool operator==(const CallablePtr&) const  = default;
		constexpr auto operator<=>(const CallablePtr&) const = default;
	};

	
	namespace Detail
	{
		template<FunctionPointer ptr_t>
		class BoundFunctionPointer
		{
		public:
			using traits_type   = FunctionTraits<ptr_t>;
			using target_type   = std::conditional_t<traits_type::is_member, typename traits_type::target_type, void*>;
			using function_type = traits_type::raw_ptr_type;
			using return_type   = traits_type::return_type;

			struct MemberParams
			{
				function_type mFunction{nullptr};
				target_type   mTarget{nullptr};
			};

			using PtrParam = std::conditional_t<traits_type::is_member, MemberParams, function_type>;
			PtrParam mPtrParam;

			consteval BoundFunctionPointer() = default;

			constexpr BoundFunctionPointer(const BoundFunctionPointer&) = default;
			BoundFunctionPointer(BoundFunctionPointer&& other)
			{
				mPtrParam       = other.mPtrParam;
				other.mPtrParam = PtrParam{};
			}

			constexpr BoundFunctionPointer(function_type func_ptr)
				requires (!traits_type::is_member)
				: mPtrParam(func_ptr)
			{
			}

			constexpr BoundFunctionPointer(function_type func_ptr, target_type target)
				requires (traits_type::is_member)
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
			constexpr return_type operator()(args_t&&... args) const noexcept(traits_type::is_noexcept)
				requires (!traits_type::is_member)
			{
				static_assert( std::invocable<function_type, args_t...> );
				return std::invoke(mPtrParam, std::forward<args_t>(args)...);
			}

			template<typename... args_t>
			constexpr return_type operator()(args_t&&... args) const noexcept(traits_type::is_noexcept)
				requires (traits_type::is_member)
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

				if constexpr (std::is_void_v<return_type>)
					std::invoke(mem_func, mTarget, std::forward<args_t>(args)...);
				else
					return std::invoke(mem_func, mTarget, std::forward<args_t>(args)...);
			}

			template<typename return_t, typename... args_t>
			operator CallablePtr<return_t(args_t...)>() const
			{
				static_assert(std::invocable<decltype(mem_func), target_t, args_t...>);

				CallablePtr<return_t(args_t...)> ret;
				ret.bind<mem_func>(mTarget);

				return ret;
			}
		};

		template<StaticFunctionPointer auto static_func>
		struct BoundFunction<static_func>
		{
			using traits               = Function<static_func>;
			using return_type          = traits::return_type;

			template<typename... args_t>
			constexpr return_type operator()(args_t&&... args) const noexcept(traits::is_noexcept)
			{
				static_assert(std::invocable<decltype(static_func), args_t...>);
				
				if constexpr (std::is_void_v<return_type>)
					std::invoke(static_func, std::forward<args_t>(args)...);
				else
					return std::invoke(static_func, std::forward<args_t>(args)...);
			}

			template<typename return_t, typename... args_t>
			operator CallablePtr<return_t(args_t...)>() const
			{
				static_assert(std::invocable<decltype(static_func), args_t...>);

				CallablePtr<return_t(args_t...)> ret;
				ret.bind<static_func>();

				return ret;
			}
		};
	}

	/**
	 * @brief
	 *  Creates an object that is callable with any arguments that <i>func</i> can be called with,
	 *  can be used in constexpr contexts, and is implicitly convertible to a CallablePtr with any signature that has
	 *  compatible argument(s) and return types.
	 */
	template<StaticFunctionPointer auto func>
	static constexpr auto bind()
	{
		return Detail::BoundFunction<func>();
	}

	/**
	 * @brief
	 *  Creates an object that is callable with any arguments that <i>func</i> can be called with,
	 *  can be used in constexpr contexts, and is implicitly convertible to a CallablePtr with any signature that has
	 *  compatible argument(s) and return types.
	 * 
	 * @param target
	 *  The object that will be bound the created functor and on which the func will be called.
	 */
	template<MemberFunctionPointer auto func>
	static constexpr auto bind(typename Function<func>::target_type target)
	{
		return Detail::BoundFunction<func>(target);
	}

	/**
	 * @brief
	 *  Creates an object that is callable with any arguments that <i>ptr</i> can be called with,
	 *  and can be used in constexpr contexts.
	 */
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

}

#endif //!_STD_EXT_CALLABLE_H_