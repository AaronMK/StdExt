#ifndef _STD_EXT_COLLECTIONS_VECTOR_H_
#define _STD_EXT_COLLECTIONS_VECTOR_H_	

#include "Collections.h"

#include "../Utility.h"
#include "../Memory.h"
#include "../Type.h"

#include <exception>
#include <variant>
#include <cstdlib>
#include <memory>
#include <array>

namespace StdExt::Collections
{
	/**
	 * @brief
	 *  A vector class that allows for finer control of internal functionality
	 *  than std::vector.
	 * 
	 *  This %Vector class provides std::vector like functionality, but
	 *  offers a finer level of control to the user.  The biggest advantage
	 *  is that a user specified number of items can be stored locally within
	 *  the container, instead of within a sperarate heap allocation.  It also
	 *  allows the user to specify granularity of block allocations for resize
	 *  operations.
	 *
	 * @tparam T
	 *  The type of elements.
	 *
	 * @tparam local_size
	 *  The number of elements for which space is reserved for local storage.
	 *  This should balance the interests of actual object size and avoiding
	 *  heap allocations.
	 *
	 * @tparam block_size
	 *  When heap allocations are necessary, the size of the space allocated
	 *  will be multiples of this parameter.  Higher values will result in more
	 *  slack space but fewer reallocations as the the number of elements grows.
	 */
	template<typename T, size_t local_size = 4, size_t block_size = 16>
	class Vector
	{
		static_assert(block_size > 0, "block_size must be greater than 0.");

		template<typename other_t, size_t other_local, size_t other_block>
		friend class Vector;

	protected:
		using buffer_t = std::conditional_t <
			local_size == 0,
			std::monostate,
			std::array<char, sizeof(T) * local_size>
		>;

		/**
		 * @brief
		 *  The number of valid elements in the container.
		 */
		size_t mSize;

		/**
		 * @brief
		 *  Information about span of data currently being used to store elements of the vector, 
		 *  including that reserved for additional elements.  When the vector has _local_size or
		 *  fewer elements (and no reserve() for more has been requested, this will point to
		 *  mLocalData.  Otherwise, this will reference the heap allocated storage.
		 */
		std::span<T> mAllocatedSpan;
		
		/**
		 * @brief
		 *  Small count storage used locally when there are local_size items or fewer.
		 */
		alignas(T) buffer_t mLocalData;

		std::span<T> activeSpan() const
		{
			return mAllocatedSpan.subspan(0, mSize);
		}

		std::span<T> localSpan() const
		{
			if constexpr (local_size > 0)
			{
				return std::span<T>(
					const_cast<T*>((const T*)&mLocalData[0]),
					local_size
				);
			}
			else
			{
				return std::span<T>();
			}
		}

		/**
		 * @brief
		 *  Returns true if the elements are stored locally within the vector.  If this is the case, a
		 *  move operation cannot simply use the mAllocatedSpan element member to take ownership of
		 *  the elements, and must perform move operations on each element instead.
		 */
		bool elementsLocal() const
		{
			return (localSpan() == mAllocatedSpan);
		}

		/**
		 * @brief
		 *  Reallocates the memory store of the elements so that it can
		 *  hold size elements.
		 *
		 *  Existing elements are moved to the new location if a new
		 *  allocation actually takes place.  When this function
		 *  returns, mAllocatedSpan will point to that new location.
		 */
		void reallocate(size_t pSize, bool shrink, bool exact)
		{
			if (pSize < mAllocatedSpan.size() && false == shrink)
				return;

			if (pSize < mSize)
				throw invalid_operation("Reallocation size requested is too small for contained elements.");

			size_t next_size;
			if (pSize <= local_size)
				next_size = local_size;
			else
				next_size = exact ? pSize : nextMutltipleOf<size_t>(pSize, block_size);

			if (mAllocatedSpan.size() != next_size)
			{
				std::span<T> destination;
				if (next_size <= local_size)
				{
					destination = localSpan();
				}
				else
				{
					destination = std::span<T>(
						allocate_n<T>(next_size),
						next_size
					);
				}

				move_n<T>(activeSpan(), destination, mSize);

				auto local_span = localSpan();

				if ( mAllocatedSpan.data() == local_span.data() && mAllocatedSpan.size() != local_span.size() )
					free_n(mAllocatedSpan.data());

				mAllocatedSpan = destination;
			}
		}

		template<size_t other_local, size_t other_block>
		void copyFrom(const Vector<T, other_local, other_block>& other)
		{
			destroy_n<T>(activeSpan());
			mSize = 0;

			reallocate(other.mSize, true, false);
			copy_n(other.activeSpan(), mAllocatedSpan, other.mSize);
			mSize = other.mSize;
		}

		template<size_t other_local, size_t other_block>
		void moveFrom(Vector<T, other_local, other_block>&& other)
		{
			destroy_n<T>(activeSpan());
			mSize = 0;

			if (other.mSize > local_size && false == other.elementsLocal())
			{
				if (false == elementsLocal())
					free_n(mAllocatedSpan.data());

				mAllocatedSpan = other.mAllocatedSpan;

				other.mAllocatedSpan = other.localSpan();
				other.DebugWatch.updateView();
				other.mSize = 0;

			}
			else
			{
				reallocate(other.mSize, true, false);
				move_n(other.activeSpan(), mAllocatedSpan, other.mSize);
				mSize = other.mSize;
				other.mSize = 0;

				if (false == other.elementsLocal())
					free_n<T>(other.mAllocatedSpan.data());

				other.mAllocatedSpan = other.localSpan();
				other.DebugWatch.updateView();
			}
		}

	public:
		Vector()
		{
			mSize = 0;
			mAllocatedSpan = localSpan();
		}

		template<size_t other_local, size_t other_block>
		Vector(Vector<T, other_local, other_block>&& other)
			: Vector()
		{
			moveFrom(std::move(other));
		}

		template<size_t other_local, size_t other_block>
		Vector(const Vector<T, other_local, other_block>& other)
			: Vector()
		{
			copyFrom(other);
		}

		virtual ~Vector()
		{
			destroy_n<T>(activeSpan());
		}

		template<size_t other_local, size_t other_block>
		Vector& operator=(Vector<T, other_local, other_block>&& other)
		{
			moveFrom(std::move(other));
			return *this;
		}

		template<size_t other_local, size_t other_block>
		Vector& operator=(const Vector<T, other_local, other_block>& other)
		{
			copyFrom(other);
			return *this;
		}

		/**
		 * @brief
		 *  Resizes the vector to size.  If smaller than the current size, elements
		 *  will be truncated and destroyed.  If greater than the current size, additional
		 *  elements will be created using arguments as construction aparameters.
		 */
		template<typename ...Args>
		void resize(size_t size, Args ...arguments)
		{
			if (size < mSize)
			{
				destroy_n(activeSpan().subspan(size));

				mSize = size;
				reallocate(mSize, true, true);
			}
			else if (size > mSize)
			{
				reallocate(size, true, true);

				for(size_t index = mSize; index < size; ++index)
					new (&mAllocatedSpan[index]) T(std::forward<Args>(arguments)...);

				mSize = size;
			}
		}

		/**
		 * @brief
		 *  The number of elements stored in the vector.
		 */
		size_t size() const
		{
			return mSize;
		}

		T& operator[](size_t index)
		{
			if (index >= mSize)
				throw std::range_error("Index out of range.");

			return mAllocatedSpan[index];
		}

		const T& operator[](size_t index) const
		{
			if (index >= mSize)
				throw std::range_error("Index out of range.");

			return mAllocatedSpan[index];
		}

		template<typename ...Args>
		void emplace_back(Args ...arguments)
		{
			reallocate(mSize + 1, false, false);
			new (&mAllocatedSpan[mSize++]) T(std::forward<Args>(arguments)...);
		}

		/**
		 * @brief
		 *  Returns true and sets result to the index of value if it is found.
		 *  Returns false and keeps result unchanged if not.
		 *
		 *  This version of find can be used to avoid the throwing of an exception
		 *  when an item is not found, and fits as something that can better be used
		 *  as a test for conditional block execution.
		 */
		bool tryFind(const T& value, size_t& result, size_t start_index = 0) noexcept
		{
			for (size_t i = start_index; i < mSize; ++i)
			{
				if (mAllocatedSpan[i] == value)
				{
					result = i;
					return true;
				}
			}

			return false;
		}

		size_t find(const T& value, size_t start_index = 0)
		{
			size_t result = 0;
			if ( tryFind(value, result, start_index) )
				return result;
			else
				throw std::range_error("Item with the specified value not found.");
		}

		void erase_at(size_t index, size_t count = 1)
		{
			if ((index + count) > mSize)
				throw std::range_error("Erase parameters exceed the bounds of the vector.");

			size_t remain_count = mSize - index - count;
			remove_n<T>(mAllocatedSpan, index, count, remain_count);
			mSize -= count;

			reallocate(mSize, true, false);
		}

		template<typename ...Args>
		void insert_n_at(size_t index, size_t count, Args ...arguments)
		{
			if (index >= mSize)
				throw std::range_error("insert index exceeds bounds of the vector.");

			size_t move_count = mSize - index;

			reallocate(mSize + count, false, false);

			insert_n(mAllocatedSpan, index, count, move_count);

			for (size_t i = 0; i < count; ++i)
				new (&mAllocatedSpan[index + i]) T(std::forward<Args>(arguments)...);

			mSize += count;
		}

		template<typename ...Args>
		void insert_at(size_t index, Args ...arguments)
		{
			insert_n_at(index, 1, std::forward<Args>(arguments)...);
		}

		/**
		 * @brief
		 *  Preallocates space for the specified number of elements
		 *  in the vector.
		 */
		void reserve(size_t count)
		{
			if (count > mAllocatedSpan.size())
				reallocate(count, false, true);
		}
	};
}

#endif // !_STD_EXT_COLLECTIONS_VECTOR_H_