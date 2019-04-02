#ifndef _STD_EXT_COLLECTIONS_H_
#define _STD_EXT_COLLECTIONS_H_	

#include "../Memory.h"

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
	static void move_n(T* source, T* destination, size_t amt)
	{
		for (size_t i = 0; i < amt; ++i)
			move_to<T>(&source[i], &destination[i]);
	}

	template<typename T>
	static void copy_n(T* source, T* destination, size_t amt)
	{
		static_assert(std::is_copy_constructible_v<T>, "T must be copy constructable.");

		for (size_t i = 0; i < amt; ++i)
			new(&destination[i]) (source[i]);
	}

	template<typename T>
	static void destroy_n(T* items, size_t amt)
	{
		for (size_t i = 0; i < amt; ++i)
			std::destroy_at<T>(&items[i]);
	}

	/**
	 * @brief
	 *  Moves count items starting at index to make way for insert_count
	 *  items at that index.
	 */
	template<typename T>
	static void insert_n(T* items, size_t index, size_t insert_count, size_t count)
	{
		T* start = &items[index];
		T* destination &items[index + insert_count];

		for (size_t i = count - 1; i >= 0; --i)
			move_to<T>(&start[i], &destination[i]);
	}

	template<typename T>
	static void remove_n(T* items, size_t index, size_t remove_count, size_t count)
	{
		T* start = &items[index];
		T* remainders = &items[index + remove_count];

		destroy_n(start, remove_count);

		for (size_t i = 0; i < count; ++i)
			move_to(&remainders[i], &start[i]);
	}
}

#endif // !_STD_EXT_COLLECTIONS_H_