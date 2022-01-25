#ifndef _STD_EXT_COLLECTIONS_H_
#define _STD_EXT_COLLECTIONS_H_	

#include "../Type.h"
#include "../Memory.h"
#include "../Concepts.h"
#include "../Exceptions.h"

#include <span>

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
	/**
	 * @brief
	 *  Allocates memory that is properly alligned and sized for amount objects of type T.  No
	 *  initialization takes place, and the memory must be deallocated by using free_n() to
	 *  avoid a memory leak.
	 */
	template<typename T>
	static T* allocate_n(size_t amount)
	{
		return reinterpret_cast<T*>(alloc_aligned(sizeof(T) * amount, alignof(T)));
	}

	/**
	 * @brief
	 *  Frees memory allocated by allocate_n.
	 */
	template<typename T>
	static void free_n(T* ptr)
	{
		free_aligned(ptr);
	}

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

	template<typename T>
		requires MoveConstructable<T> || CopyConstructable<T>
	static void move_n(std::span<T> source, std::span<T> destination, size_t amt)
	{
		if ( amt == 0 )
			return;

		if (amt > source.size() || amt > destination.size())
			throw StdExt::Exception<std::out_of_range>("Attempting to access outside the bounds of the array.");

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
		move_n<T>(std::span<T>(source, amt), std::span<T>(destination, amt), amt);
	}

	template<typename T>
		requires CopyConstructable<T>
	static void copy_n(std::span<T> source, std::span<T> destination, size_t amt)
	{
		if (amt > source.size() || amt > destination.size())
			throw StdExt::Exception<std::out_of_range>("Attempt to copy outside buffer range.");
		
		if ( memory_overlaps(source.subspan(0, amt), destination.subspan(0, amt) ) )
			throw StdExt::Exception<invalid_operation>("Cannot copy between overlapping regions of memory.");

		for (size_t i = 0; i < amt; ++i)
			new(&destination[i]) T(source[i]);
	}

	template<typename T>
		requires CopyConstructable<T>
	static void copy_n(T* source, T* destination, size_t amt)
	{
		copy_n(std::span<T>(source, amt), std::span<T>(destination, amt), amt);
	}

	template<typename T>
	static void destroy_n(std::span<T> items)
	{
		for (size_t i = 0; i < items.size(); ++i)
			std::destroy_at<T>(&items[i]);
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

		move_n<T>(itemsToMove, moveTarget, move_count);
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

		move_n<T>(itemsToMove, moveTarget, remain_count);
	}
}

#endif // !_STD_EXT_COLLECTIONS_H_