#ifndef _STD_EXT_MEMORY_SHARED_PTR_H_
#define _STD_EXT_MEMORY_SHARED_PTR_H_

#include "../Concepts.h"

#include "Casting.h"

#include <atomic>

namespace StdExt
{
	namespace Detail
	{
		struct SharedPtrControlBase
		{
			mutable std::atomic<int> refCount = 1;
			void* obj_ptr = nullptr;

			virtual ~SharedPtrControlBase() = default;
		};

		template<typename T>
		struct SharedPtrControl : public SharedPtrControlBase
		{
			T obj;

			template<typename ...args_t>
			SharedPtrControl(args_t ...arguments)
				: obj(std::forward<args_t>(arguments)...)
			{
				obj_ptr = &obj;
			}

			virtual ~SharedPtrControl() = default;
		};
	}

	template<StrippedType T>
	class SharedPtr;

	/**
	 * @brief
	 *  SharedPointer type that takes the same local space as a regular
	 *  pointer, but has some trade offs compared with std::shared_ptr.
	 *
	 *  Pros:
	 *   - Pointer object is the size of a single pointer.
	 *   - Managed object and control structure are always a single allocation.
	 *
	 *  Cons:
	 *   - For polymorphic types, dereferencing uses a virtual function call.
	 *
	 * @note
	 *  The SharedPtr<T> object itself is not thread safe, but the object it manages
	 *  maintains its thread safety charateristics.  Different SharedPtr<T> objects
	 *  referencing the same control structure reference count in a thread safe way.
	 */
	template<StrippedType T>
	class SharedPtr final
	{
		template<StrippedType U>
		friend class SharedPtr;

	private:
		Detail::SharedPtrControlBase* mControlBlock;

		void incrementBlock() const
		{
			if (mControlBlock)
				++mControlBlock->refCount;
		}

		void decrementBlock()
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
				delete mControlBlock;

			mControlBlock = nullptr;
		}

		template<typename U>
		inline void checkCast(const SharedPtr<U>& other)
		{
			static_assert(
				SubclassOf<U, T> || (InHeirarchyOf<T, U> && Polymorphic<T>) || Is<T, U>,
				"Casting between different SharedPtr types must assign to the same "
				"type, assigned to a base type, or an attempted dynamic downcast."
			);

			if constexpr (SubclassOf<T, U> && !Is<T, U>)
			{
				if (nullptr == other.mControlBlock)
					return;

				U* testPtr = access_as<U*>(other.mControlBlock->obj_ptr);
				if (nullptr == dynamic_cast<T*>(testPtr))
					throw std::bad_cast();
			}
		}

	public:
		SharedPtr()
			: mControlBlock(nullptr)
		{
		}

		template<typename U>
		SharedPtr(SharedPtr<U>&& other)
		{
			checkCast(other);

			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;
		}

		template<typename U>
		SharedPtr(const SharedPtr<U>& other)
		{
			checkCast(other);

			other.incrementBlock();
			mControlBlock = other.mControlBlock;
		}

		~SharedPtr()
		{
			decrementBlock();
		}

		template<typename U>
		SharedPtr& operator=(SharedPtr<U>&& other)
		{
			checkCast(other);
			decrementBlock();

			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;

			return *this;
		}

		template<typename U>
		SharedPtr& operator=(const SharedPtr<U>& other)
		{
			checkCast(other);
			decrementBlock();

			other.incrementBlock();
			mControlBlock = other.mControlBlock;

			return *this;
		}

		T* get()
		{
			return (mControlBlock) ?
				access_as<T*>(mControlBlock->obj_ptr) :
				nullptr;
		}

		const T* get() const
		{
			return (mControlBlock) ?
				access_as<const T*>(mControlBlock->obj_ptr) :
				nullptr;
		}

		T* operator->()
		{
			return get();
		}

		const T* operator->() const
		{
			return get();
		}

		T& operator*()
		{
			if (mControlBlock)
				return access_as<T&>(mControlBlock->obj_ptr);

			throw null_pointer("Attempting to dereference a null pointer.");
		}

		const T& operator*() const
		{
			if (mControlBlock)
				return access_as<const T&>(mControlBlock->obj_ptr);

			throw null_pointer();
		}

		operator bool() const
		{
			return (nullptr != mControlBlock);
		}

		void clear()
		{
			decrementBlock();
		}

		template<typename ...args_t>
		static SharedPtr make(args_t ...arguments)
		{
			SharedPtr ret;
			ret.mControlBlock = new Detail::SharedPtrControl<T>(std::forward<args_t>(arguments)...);

			return ret;
		}
	};
}

#endif // !_STD_EXT_MEMORY_SHARED_PTR_H_