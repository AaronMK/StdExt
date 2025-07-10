#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include "FunctionTraits.h"

#include "Memory/Casting.h"

#include <concepts>
#include <type_traits>

namespace StdExt
{
	template<SpecializationOf<FunctionTraits> traits_t>
	class BoundFunctionPointer
	{
	public:
		using target_type   = std::conditional_t<traits_t::is_member, typename traits_t::target_type, void*>;
		using function_type = traits_t::ptr_type;
		using return_type   = traits_t::return_t;

		struct MemberParams
		{
			function_type mFunction{nullptr};
			target_type   mTarget{nullptr};
		};

		using PtrParam = std::conditional_t<traits_t::is_member, MemberParams, function_type>;
		const PtrParam mPtrParam;

		consteval BoundFunctionPointer() = default;

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
	BoundFunctionPointer(ptr_t ptr, typename FunctionTraits<ptr_t>::target_type traget) -> BoundFunctionPointer< FunctionTraits<ptr_t> >;

	template<StaticFunctionPointer ptr_t>
	BoundFunctionPointer(ptr_t ptr) -> BoundFunctionPointer< FunctionTraits<ptr_t> >;

	namespace Detail
	{
		template<FunctionPointer auto func_ptr>
		struct TypeErasedJump
		{
			using func_traits = Function<func_ptr>;
			using ptr_target_type = func_traits::target_type;
			using ptr_return_type = func_traits::return_t;

			static constexpr bool ptr_is_member = func_traits::is_member;
			static constexpr bool ptr_is_noexcept = func_traits::is_noexcept;

			template<typename return_t, typename... args_t>
			static return_t jump_func(void* target, args_t&&... args)
			{
				static_assert(
					( ptr_is_member && std::invocable<decltype(func_ptr), ptr_target_type, args_t...> ) ||
					( !ptr_is_member && std::invocable<decltype(func_ptr), args_t...> ),
					"Function pointer must be invocable with argument to which it is being bound."
				);

				static_assert(
					std::convertible_to<ptr_return_type, return_t>,
					"Function pointer return type must be convertable to the requested return type."
				);

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

		template<typename return_t, typename... args_t>
		struct ObjectJump
		{
			template<bool is_const, CallableWith<return_t, args_t...> callable_t>
			static return_t jump_func(void* target, args_t&&... args)
			{
				if constexpr (is_const)
				{
					const callable_t* typed_ptr = access_as<callable_t*>(target);
					return (*typed_ptr)(std::forward<args_t>(args)...);
				}
				else
				{
					callable_t* typed_ptr = access_as<callable_t*>(target);
					return (*typed_ptr)(std::forward<args_t>(args)...);
				}
			}
		};

		template<typename return_t, typename... args_t>
		class TypeErasedDelegate
		{
		public:
			using type_erased_caller = return_t(*)(void*, args_t&&...);

			type_erased_caller Caller{nullptr};
			void* Target{nullptr};

			consteval TypeErasedDelegate() = default;
			constexpr TypeErasedDelegate(const TypeErasedDelegate&) = default;

			constexpr TypeErasedDelegate(TypeErasedDelegate&& other)
			{
				Target = other.Target;
				other.Target = nullptr;

				Caller = other.Caller;
				other.Caller = nullptr;
			}

			constexpr TypeErasedDelegate(void* _target, type_erased_caller _caller)
				: Target(_target), Caller(_caller)
			{
			}

			constexpr return_t operator()(args_t&&... args) const
			{
				return std::invoke(Caller, Target, std::forward<args_t>(args)...);
			}

			TypeErasedDelegate& operator=(const TypeErasedDelegate& other) = default;

			TypeErasedDelegate& operator=(TypeErasedDelegate&& other)
			{
				Target = other.Target;
				other.Target = nullptr;

				Caller = other.Caller;
				other.Caller = nullptr;

				return *this;
			}

			constexpr operator bool() const
			{
				return (nullptr != Caller);
			}

			constexpr bool operator==(const TypeErasedDelegate&) const  = default;
			constexpr auto operator<=>(const TypeErasedDelegate&) const = default;
		};
	}

	template<typename return_t, typename... args_t>
	using bind_return_t = Detail::TypeErasedDelegate<return_t, args_t...>;

	template<StaticFunctionPointer auto func_ptr, typename return_t, typename... args_t>
	consteval bind_return_t<return_t, args_t...> bind()
	{
		return Detail::TypeErasedDelegate<return_t, args_t...>(
			nullptr,
			&Detail::TypeErasedJump<func_ptr>::template jump_func<return_t, args_t...>
		);
	}

	template<MemberFunctionPointer auto func_ptr, typename return_t, typename... args_t>
	bind_return_t<return_t, args_t...> bind(typename Function<func_ptr>::target_type target)
	{
		return Detail::TypeErasedDelegate<return_t, args_t...>(
			access_as<void*>(target),
			&Detail::TypeErasedJump<func_ptr>::template jump_func<return_t, args_t...>
		);
	}

	template<typename return_t, typename... args_t>
	constexpr bind_return_t<return_t, args_t...> bind(Class auto* obj)
	{
		using traits = Type<decltype(obj)>;
		using core_type = traits::core;
		using obj_jump = Detail::ObjectJump<return_t, args_t...>;
		constexpr bool is_const = ConstType<decltype(obj)>;

		static_assert(
			CallableWith<core_type, return_t, args_t...>,
			"Object must be callable with the signature provided."
		);

		return Detail::TypeErasedDelegate<return_t, args_t...>(
			access_as<void*>(obj),
			&obj_jump::template jump_func<is_const, core_type>
		);
	}

	template<typename... types_t>
	class FunctionPtr;

	template<typename return_t, typename... args_t>
	class FunctionPtr<return_t(args_t...)>
	{
	private:
		bind_return_t<return_t, args_t...> mBoundFunction;

	public:
		consteval FunctionPtr() = default;

		template<MemberFunctionPointer auto func_ptr>
		void bind(Function<func_ptr>::target_type target)
		{
			mBoundFunction = bind<func_ptr, return_t, args_t...>(target);
		}

		template<StaticFunctionPointer auto func_ptr>
		void bind()
		{
			mBoundFunction = bind<func_ptr, return_t, args_t...>();
		}

		constexpr return_t operator()(args_t&&... args) const
		{
			return mBoundFunction(std::forward<args_t>(args)...);
		}

		constexpr operator bool() const
		{
			return mBoundFunction;
		}
		
		constexpr bool operator==(const FunctionPtr&) const  = default;
		constexpr auto operator<=>(const FunctionPtr&) const = default;
	};
}

#endif //!_STD_EXT_FUNCTION_PTR_H_