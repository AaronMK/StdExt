#ifndef _STD_EXT_CALLABLE_H_
#define _STD_EXT_CALLABLE_H_

#include "CallableTraits.h"

#include "Concepts.h"
#include "Utility.h"

namespace StdExt
{
	template<typename ret_t, typename... args_t>
	class Callable;

	template<typename ret_t, typename... args_t>
	class Callable
	{
	public:
		Callable() {}
		virtual ~Callable() {}

		ret_t operator()(args_t... args) const
		{
			if constexpr ( std::is_void_v<ret_t> )
				run(std::forward<args_t>(args)...);
			else
				return run(std::forward<args_t>(args)...);
		}

	protected:
		virtual ret_t run(args_t... args) const = 0;
	};

	template<typename callable_t, typename ret_t, typename... args_t>
		requires ( CallableWith<callable_t, ret_t, args_t...> )
	class Callable<callable_t, ret_t, args_t...> : public Callable<ret_t, args_t...>
	{
	public:
		Callable(const callable_t& callable)
			requires CopyConstructable<callable_t>
			: mCallable(callable)
		{
		}

		Callable(callable_t&& callable)
			requires MoveConstructable<callable_t>
			: mCallable( std::move(callable) )
		{
		}

	protected:
		ret_t run(args_t... args) const override
		{
			if constexpr ( std::is_void_v<ret_t> )
				mCallable(std::forward<args_t>(args)...);
			else
				return mCallable(std::forward<args_t>(args)...);
		}

	private:
		mutable callable_t mCallable;
	};

	template<typename callable_t>
	class Callable<callable_t> : public CallableTraits<callable_t>::template forward<Callable, callable_t>
	{
	public:
		using base_t = typename CallableTraits<callable_t>::template forward<Callable, callable_t>;

		Callable(const callable_t& func)
			: base_t(func)
		{
		}

		Callable(callable_t&& func)
			: base_t(std::move(func))
		{
		}
	};

	template<typename callable_t>
	Callable(callable_t func) -> Callable<callable_t>;

	template<typename ret_t, typename... args_t>
	class CallableRef : public Callable<ret_t, args_t...>
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
		CallableRef()
		{
			mCaller   = nullptr;
			mCallable = nullptr;
		}

		template<CallableWith<ret_t, args_t...> func_t>
		CallableRef(const func_t& func)
		{
			mCaller = &caller<func_t>;
			mCallable = access_as<void*>(&func);
		}

	protected:
		ret_t run(args_t... args) const override
		{
			if constexpr ( std::is_void_v<ret_t> )
				mCaller(mCallable, std::forward<args_t>(args)...);
			else
				return mCaller(mCallable, std::forward<args_t>(args)...);
		}
	};
}

#endif // !_STD_EXT_CALLABLE_H_