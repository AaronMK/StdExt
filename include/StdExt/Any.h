#ifndef _STD_EXT_ANY_H_
#define _STD_EXT_ANY_H_

#include "InPlace.h"
#include "Type.h"

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

			virtual const Type cw_contained_type() const = 0;
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

			virtual const Type cw_contained_type() const override
			{
				return TypeOf<T>();
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

			virtual Type cw_contained_type() const override
			{
				return TypeOf<T>();
			}
		};

		InPlace<CastWrapper, 24> mWrappedValue;

	public:

		/**
		 * @brief
		 *  Creates an empty container.
		 */
		Any()
		{
		}

		/**
		 * @brief
		 *  Moves the contained value from other into the constructed container.  If the
		 *  contained object is not move constructable, this will throw an invalid_operation
		 *  exception.
		 */
		Any(Any&& other)
			: mWrappedValue(std::move(other.mWrappedValue))
		{
		}

		/**
		 * @brief
		 *  Copies the contained value from other into the constructed container.  If the
		 *  contained object is not copy constructable, this will throw an invalid_operation
		 *  exception.
		 */
		Any(const Any& other)
			: mWrappedValue(other.mWrappedValue)
		{
		}

		/**
		 * @brief
		 *  Creates a container with an object of type T contructed
		 *  using the passed parameters.
		 */
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

		/**
		 * @brief
		 *  Attempts to dynamically cast the contents of the container to type T.  If the cast
		 *  fails, nullptr is returned.
		 */
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

		/**
		 * @brief
		 *  Attempts to dynamically cast the contents of the container to type T.  If the cast
		 *  fails, nullptr is returned.
		 */
		template<typename T>
		const T* cast() const
		{
			Any* nonConstThis = const_cast<Any*>(this);
			return nonConstThis->cast<T>();
		}

		const Type type() const
		{
			if (mWrappedValue.isEmpty())
				return TypeOf<void>();

			return mWrappedValue->cw_contained_type();
		}

		/**
		 * @brief
		 *  Runs the destructor of the contents of the container and clears it.
		 */
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