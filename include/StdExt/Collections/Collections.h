#ifndef _STD_EXT_COLLECTIONS_H_
#define _STD_EXT_COLLECTIONS_H_	

#include "../Memory/Alignment.h"
#include "../Memory/Utility.h"

#include "../Concepts.h"
#include "../Exceptions.h"
#include "../Type.h"

#include <cstring>
#include <span>
#include <memory>
#include <utility>

/**
 * @brief
 *  Collection classes with functionality differences and additions to
 *  standard library counterparts that may be advantageous and necessary
 *  for many applications.
 *
 * @details
 *  Distinctions From Standard Library Collections
 *  ------------------------
 *  - Reallocations always use move semantics when supported by the element type.
 */
namespace StdExt::Collections
{
	template<typename T, typename ...args_t>
	static void fill_uninitialized_n(std::span<T> items, args_t ...arguments)
	{
		if constexpr (std::is_trivial_v<T> && sizeof...(args_t) == 0)
			return;

		if (items.size() == 0)
			return;

		if constexpr (std::is_copy_constructible_v<T>)
		{
			::new(&items[0]) T(std::forward<args_t>(arguments)...);
			std::uninitialized_fill(items.begin() + 1, items.end(), items[0]);
		}
		else
		{
			for (size_t i = 0; i < items.size(); ++i)
				::new(&items[0]) T(std::forward<args_t>(arguments)...);
		}
	}

	/**
	 * @brief
	 *  If T is move constructable, objects from source will be used as move construction
	 *  parameters to create object at destination.  Otherwise, they will be used as copy
	 *  constructor parameters.  The objects at source, whether left invalid from a move or copies
	 *  of what is now in destination, will be destroyed after the operation.
	 *  
	 * @param source
	 *  A pointer to the start of the objects to be moved.
	 *  
	 * @param destination
	 *  A pointer to the destination of the move.  This is assumed to be unitialized, and there will
	 *  be no automatic destruction of objects that may already be there.
	 *  
	 * @param amt
	 *  The number of objects to move.
	 */
	template<typename T>
		requires MoveAssignable<T> || CopyConstructable<T>
	static void move_n(T* source, T* destination, size_t amt)
	{
		if constexpr (MemMovable<T>)
		{
			memmove(destination, source, amt * sizeof(T) );
		}
		else
		{
			bool front_first = (&source[0] > &destination[0]);

			if (front_first)
			{
				for (size_t i = 0; i < amt; ++i)
					move_to<T>(&source[i], &destination[i]);
			}
			else
			{
				for (size_t i = 1; i <= amt; ++i)
					move_to<T>(&source[amt - i], &destination[amt - i]);
			}
		}
	}

	template<typename T>
		requires MoveConstructable<T> || CopyConstructable<T>
	static void move_n(std::span<T> source, std::span<T> destination)
	{
		if ( source.size() == 0 )
			return;

		if (source.size() > destination.size())
			throw std::out_of_range("Attempt to move outside buffer range.");

		move_n(source.data(), destination.data(), source.size());
	}

	template<CopyConstructable T>
	static void copy_n(const T* source, T* destination, size_t amt)
	{
		if ( 0 == amt )
			return;

		if ( memory_overlaps(source, amt, destination, amt) )
			throw invalid_operation("Cannot copy between overlapping regions of memory.");

		if constexpr ( MemCopyable<T> )
		{
			memcpy(destination, source, amt * sizeof(T));
		}
		else
		{
			for (size_t i = 0; i < amt; ++i)
				new(&destination[i]) T(source[i]);
		}
	}

	template<CopyConstructable T>
	static void copy_n(std::span<T> source, std::span<T> destination)
	{
		if ( source.size() > destination.size() )
			throw std::out_of_range("Attempt to copy outside buffer range.");

		copy_n(source.data(), destination.data(), source.size() );
	}

	template<typename T>
	static void destroy_n(std::span<T> items)
	{
		if constexpr ( !std::is_trivially_destructible_v<T> )
		{
			for (size_t i = 0; i < items.size(); ++i)
				std::destroy_at<T>(&items[i]);
		}
	}

	template<typename T>
	static void destroy_n(T* items, size_t amt)
	{
		destroy_n<T>(std::span<T>(items, amt));
	}

	template<typename T>
	static void destroy_n(std::span<T> items, size_t start_at, size_t amt)
	{
		destroy_n<T>(items.subspan(start_at, amt));
	}

	/**
	 * @brief
	 *  Moves move_count items starting at index to make way for insert_count
	 *  items at that index.
	 *
	 * @param items
	 *  Pointer to the beginning of the item buffer.
	 * 
	 * @param index
	 *  The index at which to start the insertion.
	 *
	 * @param insert_count
	 *  The number of items for which to clear space for insertion.
	 *
	 * @param move_count
	 *  The number of items at index which will need to be moved.
	 */
	template<typename T>
	static void insert_n(T* items, size_t index, size_t insert_count, size_t move_count)
	{
		T* start = &items[index];
		T* destination = &items[index + insert_count];

		for (size_t i = 1; i <= move_count; ++i)
			move_to<T>(&start[move_count - i], &destination[move_count - i]);
	}

	/**
	 * @brief
	 *  Moves move_count items starting at index to make way for insert_count
	 *  items at that index.
	 *
	 * @param itemBuffer
	 *  A std::span covering the entire allocation on which the operation will occur.
	 *
	 * @param index
	 *  The index at which to start the insertion.
	 *
	 * @param insert_count
	 *  The number of items for which to clear space for insertion.
	 *
	 * @param move_count
	 *  The number of items at index which will need to be moved.
	 */
	template<typename T>
	static void insert_n(std::span<T> itemBuffer, size_t index, size_t insert_count, size_t move_count)
	{
		std::span<T> itemsToMove = itemBuffer.subspan(index, move_count);
		std::span<T> moveTarget = itemBuffer.subspan(index + insert_count, move_count);

		move_n<T>(itemsToMove, moveTarget);
	}

	template<typename T>
	static void remove_n(T* items, size_t index, size_t remove_count, size_t remain_count)
	{
		T* start = &items[index];
		T* remainders = &items[index + remove_count];

		destroy_n(start, remove_count);

		for (size_t i = 0; i < remain_count; ++i)
			move_to(&remainders[i], &start[i]);
	}

	template<typename T>
	static void remove_n(std::span<T> itemBuffer, size_t index, size_t remove_count, size_t remain_count)
	{
		destroy_n<T>(itemBuffer, index, remove_count);

		std::span<T> itemsToMove = itemBuffer.subspan(index + remove_count, remain_count);
		std::span<T> moveTarget = itemBuffer.subspan(index, remain_count);

		move_n<T>(itemsToMove, moveTarget);
	}

	/**
	 * @brief
	 *  Container that acts as a base for much of the funtionality of dynamically
	 *  sized containers.
	 *
	 * @tparam T
	 *  The type of elements.
	 *
	 * @tparam local_size
	 *  The number of elements for which space is reserved for local storage.
	 *  This should balance the interests of actual object size and avoiding
	 *  heap allocations.
	 * 
	 * @tparam auto_shrink
	 *  If true, the allocation of the vector will shrink to the number of blocks
	 *  needed as the number of elements shrinks.  If false, the allocation will
	 *  grow to accommodate more elements as needed or reserved, but will not shrink.
	 *
	 * @tparam block_size
	 *  When heap allocations are necessary, the size of the space allocated
	 *  will be multiples of this parameter.  Higher values will result in more
	 *  slack space but fewer reallocations as the the number of elements grows.
	 */
	template<typename T, size_t local_size = 4>
	class SlidingStorage
	{
	private:
		std::span<T> mActiveSpan;
		std::span<T> mAllocatedSpan;

		AlignedStorage<T, local_size> mLocalStorage;

		void free()
		{
			destroy_n(mActiveSpan);

			if ( !isLocal() )
			{
				free_n(mActiveSpan.data());
			}
		}

		void moveFrom(SlidingStorage&& other)
		{
			if ( this == &other )
				return;

			free();

			if ( other.isLocal() )
			{
				mAllocatedSpan = mLocalStorage.data();
				mActiveSpan    = mAllocatedSpan.subspan(
					other.leftSlack(), other.size()
				);

				move_n(other.mActiveSpan, mActiveSpan);
			}
			else
			{
				mAllocatedSpan = other.mAllocatedSpan;
				mActiveSpan    = other.mActiveSpan;
			}

			other.mAllocatedSpan = other.mLocalStorage.data();
			other.mActiveSpan    = std::span<T>(other.mAllocatedSpan.data(), 0);
		}

	public:
		void reallocate(size_t left_slack, size_t right_slack)
		{
			const size_t total_size = std::max(
				left_slack + right_slack + mActiveSpan.size(),
				mLocalStorage.size()
			);

			std::span<T> next_alloc = mAllocatedSpan;

			if ( total_size != mAllocatedSpan.size() )
			{
				next_alloc = ( total_size <= mLocalStorage.size() ) ?
					mLocalStorage.data() : 
					std::span<T>(
						allocate_n<T>(total_size),
						total_size
					);
			}

			std::span<T> next_active(
				next_alloc.data() + left_slack, mActiveSpan.size()
			);

			if ( memory_same(next_alloc, mAllocatedSpan) && memory_same(next_active, mActiveSpan) )
				return;

			move_n(mActiveSpan, next_active);

			if ( !isLocal() && !memory_same(next_alloc, mAllocatedSpan) )
				free_n(mAllocatedSpan.data());

			mAllocatedSpan = next_alloc;
			mActiveSpan    = next_active;
		}

		constexpr size_t leftSlack() const
		{
			const T* alloc_start  = mAllocatedSpan.data();
			const T* active_start = mActiveSpan.data();

			return static_cast<size_t>(active_start - alloc_start);
		}

		constexpr size_t rightSlack() const
		{
			const T* alloc_end  = mAllocatedSpan.data() + mAllocatedSpan.size();
			const T* active_end = mActiveSpan.data() + mActiveSpan.size();
			
			return static_cast<size_t>(alloc_end - active_end);
		}

		constexpr SlidingStorage() noexcept
		{
			mAllocatedSpan = mLocalStorage.data();
			mActiveSpan = std::span<T>(mAllocatedSpan.data(), 0);
		}

		SlidingStorage(const SlidingStorage& other) = delete;

		SlidingStorage(SlidingStorage&& other)
			: SlidingStorage()
		{
			moveFrom(std::move(other));
		}

		~SlidingStorage()
		{
			free();
		}

		SlidingStorage& operator=(const SlidingStorage& other) = delete;

		SlidingStorage& operator=(SlidingStorage&& other)
		{
			moveFrom(std::move(other));
			return *this;
		}

		constexpr bool isLocal() const
		{
			return memory_ecompases(
				mLocalStorage.data(), mActiveSpan
			);
		}

		constexpr size_t size() const
		{
			return mActiveSpan.size();
		}

		T& operator[](size_t index)
		{
			return mActiveSpan[index];
		}

		const T& operator[](size_t index) const
		{
			return mActiveSpan[index];
		}

		template<typename... args_t>
		void append(args_t&&... args)
		{
			if ( rightSlack() >= 1 )
			{
				mActiveSpan = std::span<T>(mActiveSpan.data(), mActiveSpan.size() + 1);
				std::construct_at<T>(&mActiveSpan.back(), std::forward<args_t>(args)...);
			}
			else
			{
				throw std::out_of_range("Attempt to write past allocated memory.");
			}
		}
	};
}

#endif // !_STD_EXT_COLLECTIONS_H_