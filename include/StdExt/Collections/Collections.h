#ifndef _STD_EXT_COLLECTIONS_H_
#define _STD_EXT_COLLECTIONS_H_	

#include "../Type.h"
#include "../Memory.h"

#include "Span.h"

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
		return reinterpret_cast<T*>(allocAligned(sizeof(T) * amount, alignof(T)));
	}

	/**
	 * @brief
	 *  Frees memory allocated by allocate_n.
	 */
	template<typename T>
	static void free_n(T* ptr)
	{
		freeAligned(ptr);
	}

	template<typename T, bool front_first = true>
	static void move_n(Span<T> source, Span<T> destination, size_t amt)
	{
		static_assert(
			Traits<T>::move_constructable || Traits<T>::copy_constructable,
			"T must be move or copy constructable."
			);

		auto dSource = Span<T>::watch(source);
		auto dDest = Span<T>::watch(destination);

		if (amt > source.size() || amt > destination.size())
			throw std::out_of_range("Attempting to access outside the bounds of the array.");

		if constexpr (front_first)
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
	template<typename T, bool front_first = true>
	static void move_n(T* source, T* destination, size_t amt)
	{
		static_assert(
			Traits<T>::move_constructable || Traits<T>::copy_constructable,
			"T must be move or copy constructable."
		);

		move_n<T, front_first>(Span<T>(source, amt), Span<T>(destination, amt), amt);
	}

	template<typename T>
	static void copy_n(Span<T> source, Span<T> destination, size_t amt)
	{
		auto dSource = Span<T>::watch(source);
		auto dDest = Span<T>::watch(destination);

		static_assert(Traits<T>::copy_constructable, "T must be copy constructable.");

		if (amt > source.size() || amt > destination.size())
			throw std::out_of_range();

		for (size_t i = 0; i < amt; ++i)
			new(&destination[i]) (source[i]);
	}

	template<typename T>
	static void copy_n(T* source, T* destination, size_t amt)
	{
		static_assert(Traits<T>::copy_constructable, "T must be copy constructable.");
		copy_n(Span<T>(source, amt), Span<T>(destination, amt), amt);
	}

	template<typename T>
	static void destroy_n(Span<T> items)
	{
		for (size_t i = 0; i < items.size(); ++i)
			std::destroy_at<T>(&items[i]);
	}

	template<typename T>
	static void destroy_n(T* items, size_t amt)
	{
		destroy_n<T>(Span<T>(items, amt));
	}

	template<typename T>
	static void destroy_n(Span<T> items, size_t start_at, size_t amt)
	{
		destroy_n<T>(items.subSpan(start_at, amt));
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
		T* destination &items[index + insert_count];

		for (size_t i = move_count - 1; i >= 0; --i)
			move_to<T>(&start[i], &destination[i]);
	}

	/**
	 * @brief
	 *  Moves move_count items starting at index to make way for insert_count
	 *  items at that index.
	 *
	 * @param itemBuffer
	 *  A span covering the entire allocation on which the operation will occur.
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
	static void insert_n(Span<T> itemBuffer, size_t index, size_t insert_count, size_t move_count)
	{
		Span<T> itemsToMove = itemBuffer.subSpan(index, move_count);
		auto dItemsToMove = Span<T>::watch(itemsToMove);

		Span<T> moveTarget = itemBuffer.subSpan(index + insert_count, move_count);
		auto dMoveTarget = Span<T>::watch(moveTarget);

		move_n<T, false>(itemsToMove, moveTarget, move_count);
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
	static void remove_n(Span<T> itemBuffer, size_t index, size_t remove_count, size_t remain_count)
	{
		destroy_n<T>(itemBuffer, index, remove_count);

		Span<T> itemsToMove = itemBuffer.subSpan(index + remove_count, remain_count);
		Span<T> moveTarget = itemBuffer.subSpan(index, remain_count);

		move_n<T>(itemsToMove, moveTarget, remain_count);
	}
}

#endif // !_STD_EXT_COLLECTIONS_H_