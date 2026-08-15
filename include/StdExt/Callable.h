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

	/**
	 * @brief
	 *  A lightweight, non-owning, type-erased callable wrapper for a function signature
	 *  <i>return_type(args_t...)</i>.  It can be bound to a callable object (functor or
	 *  lambda), a non-static member function paired with a target object, or a static/free
	 *  function, all without heap allocation.
	 *
	 * @details
	 *  Unlike std::function, CallablePtr does not own or copy the bound state - it stores
	 *  only a pointer to the target and a pointer to a dispatch function, making it trivially
	 *  copyable and cheap to pass around.  When bound to a static function it can be used in
	 *  constexpr contexts.  Because it is non-owning, the caller is responsible for ensuring
	 *  that any bound object remains valid for the lifetime of the CallablePtr.
	 *
	 * @tparam return_type
	 *  The return type of the bound callable.
	 *
	 * @tparam args_t
	 *  The argument types with which the bound callable will be invoked.
	 */
	template<typename return_type, typename... args_t>
	class CallablePtr<return_type(args_t...)>
	{
	private:
		/**
		 * @brief
		 *  Dispatch function used when bound to a callable object (functor or lambda).
		 *  Casts <i>target</i> back to a pointer of the original <i>callable_t</i>
		 *  (const-qualified if <i>is_const</i> is true) and invokes it with <i>args</i>.
		 *
		 * @tparam is_const
		 *  True if the bound object was pointed to by a const pointer, in which case
		 *  <i>target</i> is accessed as a pointer to const <i>callable_t</i>.
		 *
		 * @tparam callable_t
		 *  The concrete type of the bound callable object.
		 */
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

		/**
		 * @brief
		 *  Dispatch function used when bound to a non-static member function.  Casts
		 *  <i>target</i> back to the member function's target type and invokes <i>func</i>
		 *  on it with <i>args</i>.
		 *
		 * @tparam func
		 *  Pointer to the non-static member function to invoke.
		 */
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

		/**
		 * @brief
		 *  Dispatch function used when bound to a static or free function.  The
		 *  <i>target</i> pointer is ignored since static functions have no bound object.
		 *
		 * @tparam func
		 *  Pointer to the static or free function to invoke.
		 */
		template<StaticFunctionPointer auto func>
		static constexpr return_type static_jump(void* target, args_t&&... args)
		{
			if constexpr (std::is_void_v<return_type>)
				std::invoke(func, std::forward<args_t>(args)...);
			else
				return std::invoke(func, std::forward<args_t>(args)...);
		}

		/**
		 * @brief
		 *  The type of the dispatch functions (jump_func, member_jump, static_jump)
		 *  used to invoke the bound callable through type-erased storage.
		 */
		using jump_func_t = return_type(*)(void*, args_t&&...);

		/**
		 * @brief
		 *  Type-erased pointer to the bound target - either the bound callable object,
		 *  the bound member function's target object, or nullptr when unbound or bound
		 *  to a static function.
		 */
		void* mObj{nullptr};

		/**
		 * @brief
		 *  Pointer to the dispatch function used to invoke the bound callable, or
		 *  nullptr if no callable is currently bound.
		 */
		jump_func_t mCaller{nullptr};

	public:

		/**
		 * @brief
		 *  Alias for this fully specialized CallablePtr type, useful for referring to
		 *  the type from within generic code.
		 */
		using my_type = CallablePtr<return_type(args_t...)>;

		/**
		 * @brief
		 *  Constructs an unbound CallablePtr.
		 */
		constexpr CallablePtr() noexcept = default;

		/**
		 * @brief
		 *  Constructs an unbound CallablePtr.
		 */
		constexpr CallablePtr(nullptr_t) noexcept
		{
		};

		/**
		 * @brief
		 *  Copies the bound target and dispatch function from another CallablePtr.
		 */
		constexpr CallablePtr(const CallablePtr&) noexcept = default;

		/**
		 * @brief
		 *  Moves the bound target and dispatch function from <i>other</i>, leaving
		 *  <i>other</i> unbound.
		 */
		constexpr CallablePtr(CallablePtr&& other) noexcept
			: mObj(std::exchange(other.mObj, nullptr)),
			  mCaller(std::exchange(other.mCaller, nullptr))
		{
		}

		/**
		 * @brief
		 *  Binds to a callable object such as a functor or lambda.  The pointed-to
		 *  object is not copied and must remain valid for as long as it is used
		 *  through this CallablePtr.
		 *
		 * @tparam callable_t
		 *  The type of the callable object, deduced from <i>callable_obj</i>.  It must
		 *  be invocable with <i>args_t...</i> and return <i>return_type</i>.
		 *
		 * @param callable_obj
		 *  Pointer to the callable object to bind.
		 */
		template<CallableWith<return_type, args_t...> callable_t>
		constexpr CallablePtr(callable_t* callable_obj) noexcept
		{
			using core_t = Type<decltype(callable_obj)>::core;
			constexpr bool is_const = ConstType<decltype(callable_obj)>;

			mObj    = access_as<void*>(callable_obj);
			mCaller = &jump_func<is_const, core_t>;
		}

		/**
		 * @brief
		 *  Binds to a non-static member function that will be called on <i>target</i>.
		 *  The <i>target</i> object is not copied and must remain valid for as long as
		 *  it is used through this CallablePtr.
		 *
		 * @tparam func
		 *  Pointer to the non-static member function to bind.
		 *
		 * @param target
		 *  The object on which <i>func</i> will be called.
		 */
		template<MemberFunctionPointer auto func>
		void bind(Function<func>::target_type target) noexcept
		{
			static_assert(
				std::is_invocable_r_v<return_type, decltype(func), typename Function<func>::target_type, args_t...>,
				"func must be invocable with CallablePtr's argument types, and its result must be "
				"convertible to CallablePtr's return type."
			);

			mObj    = access_as<void*>(target);
			mCaller = &member_jump<func>;
		}

		/**
		 * @brief
		 *  Binds to a static or free function.
		 *
		 * @tparam func
		 *  Pointer to the static or free function to bind.
		 */
		template<StaticFunctionPointer auto func>
		constexpr void bind() noexcept
		{
			static_assert(
				std::is_invocable_r_v<return_type, decltype(func), args_t...>,
				"func must be invocable with CallablePtr's argument types, and its result must be "
				"convertible to CallablePtr's return type."
			);

			mObj    = nullptr;
			mCaller = &static_jump<func>;
		}

		/**
		 * @brief
		 *  Unbinds this CallablePtr, returning it to a default-constructed, unbound state.
		 */
		void clear() noexcept
		{
			mObj    = nullptr;
			mCaller = nullptr;
		}

		/**
		 * @brief
		 *  Copies the bound target and dispatch function from another CallablePtr.
		 */
		constexpr CallablePtr& operator=(const CallablePtr&) noexcept = default;

		/**
		 * @brief
		 *  Moves the bound target and dispatch function from <i>other</i>, leaving
		 *  <i>other</i> unbound.
		 */
		constexpr CallablePtr& operator=(CallablePtr&& other) noexcept
		{
			mObj    = std::exchange(other.mObj,    nullptr);
			mCaller = std::exchange(other.mCaller, nullptr);

			return *this;
		}

		/**
		 * @brief
		 *  Invokes the bound callable with <i>args</i>.  Behavior is undefined if this
		 *  CallablePtr is unbound.
		 *
		 * @param args
		 *  The arguments with which to invoke the bound callable.
		 *
		 * @return
		 *  The result of invoking the bound callable, or nothing if <i>return_type</i>
		 *  is _void_.
		 */
		constexpr return_type operator()(args_t... args) const
		{
			if constexpr (std::is_void_v<return_type>)
				std::invoke(mCaller, mObj, std::forward<args_t>(args)...);
			else
				return std::invoke(mCaller, mObj, std::forward<args_t>(args)...);
		}

		/**
		 * @brief
		 *  True if this CallablePtr is bound to a callable, false otherwise.
		 */
		constexpr operator bool() const noexcept
		{
			return (nullptr != mCaller);
		}

		/**
		 * @brief
		 *  True if this CallablePtr is bound to a callable, false otherwise.
		 */
		constexpr bool hasPointer() const noexcept
		{
			return (nullptr != mCaller);
		}

		/**
		 * @brief
		 *  True if this and <i>other</i> are bound to the same target and dispatch function.
		 */
		constexpr bool operator==(const CallablePtr&) const  = default;

		/**
		 * @brief
		 *  Compares this and <i>other</i> based on their bound target and dispatch function.
		 */
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
			BoundFunctionPointer(BoundFunctionPointer&& other) noexcept
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
			BoundFunctionPointer& operator=(BoundFunctionPointer&& other) noexcept
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
		BoundFunctionPointer(ptr_t ptr, typename FunctionTraits<ptr_t>::target_type target) -> BoundFunctionPointer<ptr_t>;

		template<StaticFunctionPointer ptr_t>
		BoundFunctionPointer(ptr_t ptr) -> BoundFunctionPointer<ptr_t>;

		template<FunctionPointer auto ptr_t>
		class BoundFunction;

		template<MemberFunctionPointer auto mem_func>
		struct BoundFunction<mem_func>
		{
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
			constexpr operator CallablePtr<return_t(args_t...)>() const
			{
				static_assert(std::invocable<decltype(mem_func), target_t, args_t...>);

				CallablePtr<return_t(args_t...)> ret;
				ret.template bind<mem_func>(mTarget);

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
			constexpr operator CallablePtr<return_t(args_t...)>() const
			{
				static_assert(std::invocable<decltype(static_func), args_t...>);

				CallablePtr<return_t(args_t...)> ret;
				ret.template bind<static_func>();

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
	constexpr auto bind()
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
	constexpr auto bind(typename Function<func>::target_type target)
	{
		return Detail::BoundFunction<func>(target);
	}

	/**
	 * @brief
	 *  Creates an object that is callable with any arguments that <i>ptr</i> can be called with,
	 *  and can be used in constexpr contexts.
	 */
	template<StaticFunctionPointer func_ptr_t>
	constexpr auto bind(func_ptr_t ptr)
	{
		return Detail::BoundFunctionPointer(ptr);
	}

	template<MemberFunctionPointer func_ptr_t>
	constexpr auto bind(func_ptr_t ptr, typename FunctionTraits<func_ptr_t>::target_type target)
	{
		return Detail::BoundFunctionPointer(ptr, target);
	}

}

#endif //!_STD_EXT_CALLABLE_H_