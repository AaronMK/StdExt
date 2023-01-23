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
			std::atomic<int> refCount = 1;

			virtual ~SharedPtrControlBase() = default;
			virtual void* ptr() = 0;
		};

		template<Polymorphic T>
		struct SharedPtrControl : public SharedPtrControlBase
		{
			T item;

			template<typename ...args_t>
			SharedPtrControl(args_t ...arguments)
				: item(std::forward<args_t>(arguments)...)
			{
			}

			virtual ~SharedPtrControl() = default;

			void* ptr() override
			{
				return &item;
			};
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

		struct BasicControlBlock
		{
			T item;
			std::atomic<int> refCount = 1;

			template<typename ...args_t>
			BasicControlBlock(args_t ...arguments)
				: item(std::forward<args_t>(arguments)...)
			{
			}

			inline void* ptr()
			{
				return &item;
			}
		};

		using block_ptr_t = std::conditional_t< Polymorphic<T>,
			Detail::SharedPtrControlBase*, BasicControlBlock* >;

		mutable block_ptr_t mControlBlock{};

		void incrementBlock() const
		{
			if (mControlBlock)
				++mControlBlock->refCount;
		}

		void decrementBlock() const
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
				delete mControlBlock;

			mControlBlock = nullptr;
		}

		template<typename U>
		inline void checkCast(const SharedPtr<U>& other)
		{
			static_assert(
				(InHeirarchyOf<T, U> && Polymorphic<T> && Polymorphic<U>) || Is<T, U>,
				"Casting between different SharedPtr types must be between polymorphic types "
				"in the same class hierachy of each other."
			);

			if constexpr (SubclassOf<T, U> && !Is<T, U>)
			{
				if (nullptr == other.mControlBlock)
					return;

				U* testPtr = access_as<U*>(other.mControlBlock->ptr());
				if (nullptr == dynamic_cast<T*>(testPtr))
					throw std::bad_cast();
			}
		}

	public:
		SharedPtr()
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
				access_as<T*>(mControlBlock->ptr()) :
				nullptr;
		}

		const T* get() const
		{
			return (mControlBlock) ?
				access_as<const T*>(mControlBlock->ptr()) :
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
				return access_as<T&>(mControlBlock->ptr());

			throw null_pointer("Attempting to dereference a null pointer.");
		}

		const T& operator*() const
		{
			if (mControlBlock)
				return access_as<const T&>(mControlBlock->ptr());

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
			
			if constexpr ( Polymorphic<T> )
				ret.mControlBlock = new Detail::SharedPtrControl<T>(std::forward<args_t>(arguments)...);
			else
				ret.mControlBlock = new BasicControlBlock(std::forward<args_t>(arguments)...);

			return ret;
		}
	};
}

#endif // !_STD_EXT_MEMORY_SHARED_PTR_H_