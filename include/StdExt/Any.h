#ifndef _STD_EXT_ANY_H_
#define _STD_EXT_ANY_H_

#include "InPlace.h"
#include "Concepts.h"
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

			virtual const std::type_info& typeInfo() const = 0;
			virtual std::type_index typeIndex() const = 0;
		};

		template<typename T>
		class WrappedObject : public CastWrapper, public T
		{
		public:
			template<typename ...Args>
			WrappedObject(Args ...arguments)
				: T(std::forward<Args>(arguments)...)
			{
			}

			WrappedObject(const WrappedObject& other) requires std::copy_constructible<T>
				: T(other)
			{
			}

			WrappedObject(WrappedObject&& other) requires std::move_constructible<T>
				: T(std::move(other))
			{
			}

			virtual ~WrappedObject()
			{
			}

			virtual const std::type_info& typeInfo() const override
			{
				return typeid(T);
			}

			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(T));
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

			virtual const std::type_info& typeInfo() const override
			{
				return typeid(T);
			}

			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(T));
			}
		};

		InPlace<CastWrapper, 24> mWrappedValue;

	public:
		Any(Any&& other) = default;
		Any& operator=(Any&& other) = default;

		Any(const Any& other) = default;
		Any& operator=(const Any& other) = default;

		/**
		 * @brief
		 *  Creates an empty container.
		 */
		Any()
		{
		}

		/**
		 * @brief
		 *  Creates a container with an object of type T contructed
		 *  using the passed parameters.
		 */
		template<NonConstType T, typename ...Args>
		void setValue(Args ...arguments)
		{
			if constexpr (std::is_same_v<T, StringLiteral>)
			{
				String str = StringLiteral(std::forward<Args>(arguments)...);
				mWrappedValue.setValue<WrappedObject<String>>(str);
			}
			else if constexpr (std::is_class_v<T>)
			{
				mWrappedValue.setValue<WrappedObject<T>>(std::forward<Args>(arguments)...);
			}
			else
			{
				mWrappedValue.setValue<WrappedPrimitive<T>>(std::forward<Args>(arguments)...);
			}
		}

		/**
		 * @brief
		 *  Attempts to dynamically cast the contents of the container to type T.  If the cast
		 *  fails, nullptr is returned. Otherwise a casted pointer to the contents is returned.
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
		 *  fails, nullptr is returned.  Otherwise a casted pointer to the contents is returned.
		 */
		template<typename T>
		const T* cast() const
		{
			Any* nonConstThis = const_cast<Any*>(this);
			return nonConstThis->cast<T>();
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

		bool isEmpty() const
		{
			return mWrappedValue.isEmpty();
		}

		const std::type_info& typeInfo() const
		{
			return isEmpty() ? 
				typeid(void) :
				mWrappedValue->typeInfo();
		}

		virtual std::type_index typeIndex() const
		{
			return isEmpty() ? 
				std::type_index(typeid(void)) :
				mWrappedValue->typeIndex();
		}
	};
}

#endif // _STD_EXT_ANY_H_