#ifndef _STD_EXT_ANY_H_
#define _STD_EXT_ANY_H_

#include "InPlace.h"

#include <type_traits>

namespace StdExt
{
	/**
	 * @brief
	 *  Much like std::any, this container can store a value of any type.
	 *  It has the advantage of working dynamic casting to any type in
	 *  the hierarchy of the contained type.
	 */
	class Any final
	{
		class CastWrapper
		{
		public:
			CastWrapper() {}
			virtual ~CastWrapper() {}

			virtual const std::type_info& type() const = 0;
		};

		template<typename T>
		class WrappedObject : public CastWrapper, public T
		{
		public:
			template<typename ...Args>
			WrappedObject(Args&& ...arguments)
				: T(std::forward<Args>(arguments)...)
			{
			}

			virtual ~WrappedObject()
			{
			}

			virtual const std::type_info& type() const override
			{
				return typeid(T);
			}
		};

		template<typename T>
		class WrappedPrimitive : public CastWrapper
		{
		public:
			T mValue;

			WrappedPrimitive()
			{
			}

			WrappedPrimitive(T val)
				: mValue(val)
			{
			}

			virtual ~WrappedPrimitive()
			{
			}

			virtual const std::type_info& type() const override
			{
				return typeid(T);
			}
		};

		InPlace<CastWrapper, 24> mWrappedValue;

	public:
		Any()
		{
		}

		template<typename T, typename ...Args>
		static Any make(Args&& ...arguments)
		{
			Any ret;

			if constexpr (std::is_class_v<T>)
			{
				ret.mWrappedValue.setValue<WrappedObject<T>>(std::forward<Args>(arguments)...);
			}
			else
			{
				ret.mWrappedValue.setValue<WrappedPrimitive<T>>(std::forward<Args>(arguments)...);
			}

			return ret;
		}

		template<typename T>
		T* cast()
		{
			if (mWrappedValue.isEmpty())
				return nullptr;

			if constexpr (std::is_class_v<T>)
			{
				return mWrappedValue.cast<T>();
			}
			else
			{
				WrappedPrimitive<T>* wrappedPrimitive = mWrappedValue.cast<WrappedPrimitive<T>>();

				if (wrappedPrimitive)
					return &wrappedPrimitive->mValue;
				else
					return nullptr;
			}
		}

		template<typename T>
		const T* cast() const
		{
			Any* nonConstThis = static_cast<Any*>(this);
			return nonConstThis->cast<T>();
		}

		const std::type_info& type() const
		{
			if (mWrappedValue.isEmpty())
				return typeid(void);

			return mWrappedValue->type();
		}

		void clear()
		{
			mWrappedValue.clear();
		}

		bool canMove() const
		{
			return mWrappedValue.canMove();
		}

		bool canCopy() const
		{
			return mWrappedValue.canCopy();
		}
	};

	template<typename T, typename ...Args>
	static Any make_any(Args&& ...arguments)
	{
		return Any::make<T>(std::forward<Args>(arguments)...);
	}
}

#endif // _STD_EXT_ANY_H_