#ifndef _STD_EXT_COLLECTIONS_VECTOR_H_
#define _STD_EXT_COLLECTIONS_VECTOR_H_	

#include "Collections.h"

#include "../Utility.h"
#include "../Memory.h"
#include "../Type.h"

#include <exception>
#include <cstdlib>
#include <memory>

namespace StdExt::Collections
{
	template<typename T>
	class Vector
	{
	protected:

		/**
		 * @brief
		 *  The number of valid elements in the container.
		 */
		size_t mSize;

		/**
		 * @brief
		 *  The number of elements allowed by the current allocation.
		 */
		size_t mAllocatedSize;
		
		/**
		 * @brief
		 *  A pointer to the first contained element.  This will be nullptr when empty.  When
		 *  elementsLocal() is false, this points to memory allocated using allocate_n() with
		 *  mSize valid elements and is of mAllocatedSize.  It is freed using free_n().
		 */
		T* mElements;

		static constexpr size_t DEFAULT_BLOCK_SIZE = 16;

	public:

		Vector()
		{
			mSize = 0;
			mAllocatedSize = 0;
			mElements = nullptr;
		}

		Vector(Vector<T>&& other)
			: Vector()
		{
			if (nullptr != other.mElements)
			{
				mSize = other.mSize;
				mAllocatedSize = other.mAllocatedSize;

				if (other.elementsLocal())
				{
					mAllocatedSize = nextMutltipleOf(mSize, DEFAULT_BLOCK_SIZE);
					mElements = allocate_n<T>(mAllocatedSize);
					move_n(other.mElements, mElements, mSize);
				}
				else
				{
					mElements = movePtr(other.mElements);
				}

				other.mAllocatedSize = 0;
				other.mElements = nullptr;
				other.mSize = 0;
			}
		}

		Vector(const Vector<T>& other)
		{
			mSize = other.mSize;
			mAllocatedSize = nextMutltipleOf(mSize, DEFAULT_BLOCK_SIZE);
			mElements = allocate_n<T>(mAllocatedSize);
			copy_n<T>(other.mElements, mElements, other.mSize);
		}

		virtual ~Vector()
		{
			if (mElements)
			{
				destroy_n<T>(mElements, mSize);

				if (!elementsLocal())
					free_n(mElements);
			}
		}

		template<typename ...Args>
		void resize(size_t size, Args ...arguments)
		{
			if (size < mSize)
			{
				for(size_t index = mSize - 1; index >= size; --index)
					std::destroy_at<T>(&mElements[index]);

				mSize = size;
				reallocate(mSize, true, false);
			}
			else if (size > mSize)
			{
				reallocate(size, true, false);

				for(size_t index = mSize; index < size; ++index)
					new (&mElements[index]) T(std::forward<Args>(arguments)...);

				mSize = size;
			}
		}

		size_t size() const
		{
			return mSize;
		}

		T& operator[](size_t index)
		{
			if (index >= mSize)
				throw std::range_error("Index out of range.");

			return mElements[index];
		}

		const T& operator[](size_t index) const
		{
			if (index >= mSize)
				throw std::range_error();

			return mElements[i];
		}

		template<typename ...Args>
		void emplace_back(Args ...arguments)
		{
			reallocate(mSize + 1, false, false);
			new (&mElements[mSize++]) T(std::forward<Args>(arguments)...);
		}

		size_t find(const T& value, size_t start_index = 0)
		{
			for (size_t i = start_index; i < mSize; ++i)
			{
				if (mElements[i] == value)
					return i;
			}

			throw std::range_error("Item with the specified value not found.");
		}

		void erase_at(size_t index, size_t count = 1)
		{
			if ((index + count) > mSize)
				throw std::range_error("Erase parameters exceed the bounds of the vector.");

			destroy_n<T>(&mElements[index], count);

			size_t move_start_index = index + count;
			size_t move_count = mSize - move_start_index;

			move_n<T>(&mElements[move_start_index], &mElements[index], move_count);

			mSize -= count;
			reallocate(mSize, true, false);
		}

	protected:

		/**
		 * @brief
		 *  Returns true if the elements are stored locally within the vector.  If this is the case, a
		 *  move operation cannot simply use the mElements element member to take ownership of
		 *  the elements, and must perform move operations on each element instead.
		 */
		virtual bool elementsLocal() const
		{
			return false;
		}

		/**
		 * @brief
		 *  Reallocates the memory store of the elements so that it can
		 *  hold size elements.
		 *  
		 * @details
		 *  Existing elements are moved to the new location if a new
		 *  allocation actually takes place.  When this function
		 *  returns, mElements will point to that new location.
		 */
		virtual void reallocate(size_t pSize, bool shrink, bool exact)
		{
			if (pSize < mAllocatedSize && false == shrink)
				return;

			if (pSize < mSize)
				throw invalid_operation("Reallocation size requested is too small for contained elements.");

			size_t nextSize = exact ? pSize : nextMutltipleOf(pSize, DEFAULT_BLOCK_SIZE);

			if (nextSize != mAllocatedSize)
			{
				T* nextElements = allocate_n<T>(nextSize);

				if (mElements)
				{
					move_n<T>(mElements, nextElements, mSize);
					free_n(mElements);
				}

				mElements = nextElements;
			}
		}
	};

	template<typename T, size_t local_size = 0, size_t block_size = 16>
	class AdvVector : public Vector<T>
	{
	private:
		alignas(T) std::array<std::byte, local_size * sizeof(T)> mLocalBuffer;

	public:
		using my_type = AdvVector<T, local_size, block_size>;
	};
}

#endif // !_STD_EXT_COLLECTIONS_VECTOR_H_