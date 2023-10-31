#ifndef _STD_EXT_CALLABLE_H_
#define _STD_EXT_CALLABLE_H_

#include "Callable.h"

#include "CallableTraits.h"
#include "Concepts.h"
#include "Exceptions.h"
#include "Utility.h"

#include <memory>

namespace StdExt
{
	/**
	 * @rbief
	 *  Encapsulates a callable object or a function call and its
	 *  captured data.  This is done within the object itself without
	 *  any allocating of data external to the object.
	 * 
	 *  Specializations implement different use cases.  Its default
	 *  specialization can be directly inherireted, and overriding
	 *  the protected ret_t run(args_t...) const method will provide
	 *  the needed functionality.
	 * 
	 *  Otherwise, using callable_t and passing them to the Callable
	 *  constructor will match the specialization that wraps the callable
	 *  object and implement run() automatically is easiest.
	 * 
	 * @code
	 *  // Subclass example
	 * class TestCallable : public StdExt::Callable<int, int>
	 *	{
	 *	public:
	 *		TestCallable() {}
	 *	
	 *	protected:
	 *		int run(int i) const override
	 *		{
	 *			return i + 1;
	 *		}
	 *	};
	 *	
	 *	 // Construct through deducation example:
	 *	 auto call = StdExt::Callable(
	 *		[](int i)
	 *		{
	 *			return i + 1;
	 *		}
	 *	);
	 * @endcode
	 */
	template<typename ret_t, typename... args_t>
	class Callable;

	template<typename ret_t, typename... args_t>
	class Callable
	{
	public:
		ret_t operator()(args_t... args) const
		{
			if constexpr ( std::is_void_v<ret_t> )
			{
				run(std::forward<args_t>(args)...);
			}
			else
			{
				return run(std::forward<args_t>(args)...);
			}
		}

	protected:
		virtual ret_t run(args_t... args) const = 0;
	};

	template<typename callable_t, typename ret_t, typename ...args_t>
		// requires ( CallableWith<callable_t, ret_t, args_t...> )
	class CallableImp : public Callable<ret_t, args_t...>
	{
	public:

		template<typename... construct_args_t>
		CallableImp(construct_args_t&& ...args)
			: mCallable(std::forward<construct_args_t>(args)...)
		{
		}

	protected:
		ret_t run(args_t... args) const override
		{
			if constexpr ( std::is_void_v<ret_t> )
			{
				mCallable(std::forward<args_t>(args)...);
			}
			else
			{
				return mCallable(std::forward<args_t>(args)...);
			}
		}

	private:
		callable_t mCallable;
	};

	template<typename callable_t>
	class Callable<callable_t> : public CallableTraits<callable_t>::template forward<CallableImp, callable_t>
	{
	public:
		using base_t = typename CallableTraits<callable_t>::template forward<CallableImp, callable_t>;

		Callable(const callable_t& func)
			: base_t(func)
		{
		}

		Callable(callable_t&& func)
			: base_t( std::move(func))
		{
		}
	};

	template<typename callable_t>
	Callable(callable_t) -> Callable<callable_t>;

	namespace details
	{
		template<typename ret_t, typename... args_t>
		class OpaqueCaller
		{
		private:
			using call_ptr_t = ret_t(*)(void*, args_t...);
			const call_ptr_t mCallerFunc;

			template<typename callable_t>
			static ret_t caller(void* func, args_t... args)
			{
				callable_t& func_ref = access_as<callable_t&>(func);

				if constexpr ( std::is_void_v<ret_t> )
					func_ref(std::forward<args_t>(args)...);
				else
					return func_ref(std::forward<args_t>(args)...);
			}

		public:
			template<CallableWith<ret_t, args_t...> callable_t>
			constexpr OpaqueCaller()
				: mCallerFunc(&caller<callable_t>)
			{
			}

			ret_t call(void* func, args_t... args) const
			{
				if constexpr ( std::is_void_v<ret_t> )
					mCallerFunc(func, std::forward<args_t>(args)...);
				else
					return mCallerFunc(func, std::forward<args_t>(args)...);
			}
		};
	}

	/**
	 * @brief
	 *  Stores a pointer to a callable type and the code to forward invokations of its
	 *  own call operator to the callable object to which it currently points.
	 *
	 * @details
	 *  Some use cases:
	 *
	 *  A parameter type for functions that can take a pointer to a callable without needing
	 *  to template the function, or rely on std::function as a bridge and the potential
	 *  memory allocation:
	 * 
	 * @code
	 *	int foo(StdExt::CallableRef<int, int> func)
	 *	{
	 *		return func(1);
	 *	}
	 * 
	 *	int tmp = foo(
	 *		[](int i)
	 *		{
	 *			return i + 1;
	 *		}
	 *	);
	 * @endcode
	 * 
	 *  A function pointer type that be dynamically set to point to direrent function types
	 *  as on as they have the same signature:
	 * 
	 * @code
	 *	auto lambda_plus_one = [](int i)
	 *		{
	 *			return i + 1;
	 *		};
	 *	
	 *	auto lambda_plus_two = [](int i)
	 *		{
	 *			return i + 2;
	 *		};
	 *	
	 *	StdExt::CallableRef<int, int> call_ref;
	 *	
	 *	call_ref = lambda_plus_one;
	 *	int result = call_ref(1);
	 *	
	 *	call_ref = lambda_plus_two;
	 *	result = call_ref(1);
	 * @endcode
	 * 
	 * Some notes:
	 *  - Encapsuting pointers to non-static member functions are not supported.  The preferred method is to
	 *    wrap the call into a lambda with the target object captured.
	 * 
	 *  - When using as a replacement for a templated parameter type, there is more overhead in making a
	 *    function call.  It has to dereference both a pointer to the callable object and a pointer to
	 *    a function that will implement the actual call, and fewer optimization oportunities.  However,
	 *    this will allow for implmenting the calling code outside of a template definition.
	 */
	template<typename ret_t, typename... args_t>
	class CallableRef
	{
	private:
		using call_ptr_t = ret_t(*)(void*, args_t...);

		void* mCallable;
		call_ptr_t mCaller;
		
		template<typename callable_t>
		static ret_t caller(void* func, args_t... args)
		{
			callable_t& func_ref = access_as<callable_t&>(func);

			if constexpr ( std::is_void_v<ret_t> )
				func_ref(std::forward<args_t>(args)...);
			else
				return func_ref(std::forward<args_t>(args)...);
		}

	public:
		constexpr CallableRef()
		{
			mCaller   = nullptr;
			mCallable = nullptr;
		}

		template<CallableWith<ret_t, args_t...> func_t>
		constexpr CallableRef(const func_t& func)
		{
			mCaller = &caller<func_t>;
			mCallable = access_as<void*>(&func);
		}

		ret_t operator()(args_t... args) const
		{
			if ( nullptr == mCallable )
				throw null_pointer("Attempting to call null CallableRef");

			if constexpr ( std::is_void_v<ret_t> )
				mCaller(mCallable, std::forward<args_t>(args)...);
			else
				return mCaller(mCallable, std::forward<args_t>(args)...);
		}
	};
}

#endif // !_STD_EXT_CALLABLE_H_