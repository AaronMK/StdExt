#ifndef _STD_EXT_FUNCTION_PTR_H_
#define _STD_EXT_FUNCTION_PTR_H_

#include <type_traits>
#include <algorithm>

#include "Exceptions.h"
#include "Memory.h"
#include "Utility.h"

namespace StdExt
{
	/**
	 * @brief
	 *  This class stores a function pointer of any type and provides
	 *  a function call operator with less overhead than std::function, but
	 *  does not provide any capture capabilities.
	 */
	template<typename return_t, typename... args_t>
	class FunctionPtr
	{
	public:
		using my_t = FunctionPtr<return_t, args_t...>;

		using static_t = return_t (*)(args_t...);

		template<typename class_t>
		using member_t = return_t (class_t::*)(args_t...);

		template<typename class_t>
		using const_member_t = return_t (class_t::*)(args_t...) const;

	private:
		class Functor
		{
		public:
			virtual return_t call(args_t...) const = 0;
		};

		class StaticFunctor : public Functor
		{
		private:
			static_t mFuncPtr;

		public:
			StaticFunctor(static_t ptr)
				: mFuncPtr(ptr)
			{
			}

			virtual return_t call(args_t ...arguments) const override
			{
				if constexpr (std::is_void_v<return_t>)
					mFuncPtr(std::forward<args_t>(arguments)...);
				else
					return mFuncPtr(std::forward<args_t>(arguments)...);
			}
		};

		template<typename class_t>
		class MemberFunctor : public Functor
		{
		private:
			member_t<class_t> mFuncPtr;
			class_t* mObj;

		public:
			MemberFunctor(member_t<class_t> func, class_t* obj)
				: mFuncPtr(func), mObj(obj)
			{
			}

			virtual return_t call(args_t ...arguments) const override
			{
				class_t* nonConstObj = const_cast<class_t*>(mObj);

				if constexpr (std::is_void_v<return_t>)
					(nonConstObj->*mFuncPtr)(std::forward<args_t>(arguments)...);
				else
					return (nonConstObj->*mFuncPtr)(std::forward<args_t>(arguments)...);
			}
		};

		template<typename class_t>
		class ConstMemberFunctor : public Functor
		{
		private:
			const_member_t<class_t> mFuncPtr;
			class_t* mObj;

		public:
			ConstMemberFunctor(const_member_t<class_t> func, class_t* obj)
				: mFuncPtr(func), mObj(obj)
			{
			}

			virtual return_t call(args_t ...arguments) const override
			{
				if constexpr (std::is_void_v<return_t>)
					(mObj->*mFuncPtr)(std::forward<args_t>(arguments)...);
				else
					return (mObj->*mFuncPtr)(std::forward<args_t>(arguments)...);
			}
		};

		static constexpr size_t data_size = sizeof(MemberFunctor<EmptyClass>);

		char data[data_size];

		#if defined(STD_EXT_DEBUG)
		/**
		 * @brief
		 *  A pointer to the Functor only active in debug builds to provide visibility of the
		 *  value in the debugger.
		 */
		Functor* mFunctor = nullptr;

		inline void initDebug()
		{
			mFunctor = reinterpret_cast<Functor*>(&data[0]);
		}
		#else
		inline void initDebug()
		{
		}
		#endif

	public:
		FunctionPtr() noexcept
		{
			initDebug();
			memset(&data[0], 0, sizeof(data));
		}

		FunctionPtr(const my_t& other) noexcept
		{
			initDebug();
			memcpy(&data[0], &other.data[0], data_size);
		}

		FunctionPtr(my_t&& other) noexcept
		{
			initDebug();

			memcpy(&data[0], &other.data[0], data_size);
			other.clear();
		}

		FunctionPtr(static_t func) noexcept
		{
			initDebug();

			if (nullptr == func)
				memset(&data[0], 0, sizeof(data));
			else
				new (&data[0]) StaticFunctor(func);
		}

		template<typename class_t>
		FunctionPtr(member_t<class_t> func, class_t* obj) noexcept
		{
			initDebug();

			if (nullptr == func || nullptr == obj)
				memset(&data[0], 0, sizeof(data));
			else
				new (&data[0]) MemberFunctor(func, obj);
		}

		template<typename class_t>
		FunctionPtr(const_member_t<class_t> func, class_t* obj) noexcept
		{
			initDebug();

			if (nullptr == func || nullptr == obj)
				memset(&data[0], 0, sizeof(data));
			else
				new (&data[0]) ConstMemberFunctor(func, obj);
		}

		my_t& operator=(const my_t& other)
		{
			memcpy(&data[0], &other.data[0], data_size);
			return *this;
		}

		my_t& operator=(my_t&& other) noexcept
		{
			memcpy(&data[0], &other.data[0], data_size);
			other.clear();

			return *this;
		}

		void clear()
		{
			memset(&data[0], 0, sizeof(data));
		}

		void bind(static_t func)
		{
			new (&data[0]) StaticFunctor(func);
		}

		/**
		 * @brief
		 *  Binds a non-static member function to the pointer.
		 *
		 * @param func
		 *  A pointer to the member function.
		 *
		 * @param obj
		 *  A pointer to the object on which the function will
		 *  be called.
		 */
		template<typename class_t>
		void bind(member_t<class_t> func, class_t* obj)
		{
			new (&data[0]) MemberFunctor<class_t>(func, obj);
		}

		/**
		 * @brief
		 *  Binds a constant non-static member function pointer.
		 *
		 * @param func
		 *  A pointer to the member function.
		 *
		 * @param obj
		 *  A pointer to the object on which the function will
		 *  be called.
		 */
		template<typename class_t>
		void bind(const_member_t<class_t> func, class_t* obj)
		{
			new (&data[0]) ConstMemberFunctor<class_t>(func, obj);
		}

		return_t operator()(args_t ...arguments) const
		{
			uintptr_t* testPtr = force_cast_pointer<uintptr_t*>(&data[0]);

			if (0 == *testPtr)
				throw null_pointer();

			const Functor* func = reinterpret_cast<const Functor*>(&data[0]);

			if constexpr (std::is_void_v<return_t>)
				func->call(std::forward<args_t>(arguments)...);
			else
				return func->call(std::forward<args_t>(arguments)...);
		}

		/**
		 * @brief
		 *  Bool conversion evaluates to true if function pointer is non-null.
		 */
		operator bool() const
		{
			uintptr_t* testPtr = force_cast_pointer<uintptr_t*>(&data[0]);
			return (0 != *testPtr);
		}
	};
}

#endif //!_STD_EXT_FUNCTION_PTR_H_