#ifndef _STD_EXT_COLLECTIONS_VECTOR_H_
#define _STD_EXT_COLLECTIONS_VECTOR_H_	

#include "Collections.h"

#include "../Utility.h"
#include "../Memory.h"
#include "../Type.h"

#include "../Debug/Debug.h"

#include <exception>
#include <variant>
#include <cstdlib>
#include <memory>

namespace StdExt::Collections
{
	template<typename T, size_t local_size = 0, size_t block_size = 16>
	class Vector
	{
		static_assert(block_size > 0, "block_size must be grater than 0.");

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

		Span<T> mAllocatedSpan;

		alignas(T) buffer_t mLocalData;

		Debug::ArrayWatch<T> DebugWatch;

		Span<T> activeSpan() const
		{
			return mAllocatedSpan.subSpan(0, mSize);
		}

		Span<T> localSpan() const
		{
			if constexpr (local_size > 0)
			{
				return Span<T>(
					const_cast<T*>((const T*)&mLocalData[0]),
					local_size
				);
			}
			else
			{
				return Span<T>();
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
		 * @details
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
				Span<T> destination;
				if (next_size <= local_size)
				{
					destination = localSpan();
				}
				else
				{
					destination = Span<T>(
						allocate_n<T>(next_size),
						next_size
					);
				}

				move_n<T>(activeSpan(), destination, mSize);

				if (mAllocatedSpan != localSpan())
					free_n(mAllocatedSpan.data());

				mAllocatedSpan = destination;
				DebugWatch.updateView();

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
				DebugWatch.updateView();

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
			: DebugWatch(Span<T>::watch(mAllocatedSpan))
		{
			mSize = 0;
			mAllocatedSpan = localSpan();
			DebugWatch.updateView();
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

		template<typename ...Args>
		void resize(size_t size, Args ...arguments)
		{
			if (size < mSize)
			{
				destroy_n(activeSpan().subSpan(size));

				mSize = size;
				reallocate(mSize, true, false);
			}
			else if (size > mSize)
			{
				reallocate(size, true, false);

				for(size_t index = mSize; index < size; ++index)
					new (&mAllocatedSpan[index]) T(std::forward<Args>(arguments)...);

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

			return mAllocatedSpan[index];
		}

		const T& operator[](size_t index) const
		{
			if (index >= mSize)
				throw std::range_error("Index out of range.");

			return mAllocatedSpan[i];
		}

		template<typename ...Args>
		void emplace_back(Args ...arguments)
		{
			reallocate(mSize + 1, false, false);
			new (&mAllocatedSpan[mSize++]) T(std::forward<Args>(arguments)...);
		}

		size_t find(const T& value, size_t start_index = 0)
		{
			for (size_t i = start_index; i < mSize; ++i)
			{
				if (mAllocatedSpan[i] == value)
					return i;
			}

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

			reallocate(mSize + count, true, false);

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
	};
}

#endif // !_STD_EXT_COLLECTIONS_VECTOR_H_