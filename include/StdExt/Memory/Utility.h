#ifndef _STD_EXT_MEMORY_H_
#define _STD_EXT_MEMORY_H_

#include "../Concepts.h"
#include "../Memory/Casting.h"

#include "Alignment.h"

#include <type_traits>
#include <cstddef>
#include <memory>
#include <span>


#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

namespace StdExt
{
	/**
	 * @brief
	 *  Returns true if the passed regions of memory overlap.
	 * 
	 *  When non-void pointer types are passed, regions are detailly calculated,
	 *  based on the type and count.  When void pointers are passed
	 * 
	 * @param start_1
	 *  A pointer to the first element in the first memory range.
	 * 
	 * @param size_1
	 *  The number of elements in the first memory range.
	 * 
	 * @param start_2
	 *  A pointer to the first element in the second memory range.
	 * 
	 * @param size_2
	 *  The number of elements in the second memory range.
	 */
	template<PointerType T, PointerType U>
	static constexpr bool memory_overlaps(const T start_1, size_t size_1, const U start_2, size_t size_2)
	{
		if constexpr (std::is_same_v<T, void*> && std::is_same_v<U, void*>)
		{
			size_t left_begin = (size_t)start_1;
			size_t left_end = left_begin + size_1 - 1;
			size_t right_begin = (size_t)start_2;
			size_t right_end = right_begin + size_2 - 1;

			return (
				(left_begin >= right_begin && left_begin <= right_end) ||
				(right_begin >= left_begin && right_begin <= left_end) ||
				(left_end >= right_begin && left_end <= right_end) ||
				(right_end >= left_begin && right_end <= left_end)
			);
		}
		else
		{
			constexpr size_t elm_size_1 = 
				AnyOf<T, void*, void* const, const void*, const void* const> ?
				sizeof(std::byte) : sizeof(std::remove_pointer_t<T>);

			constexpr size_t elm_size_2 =
				AnyOf<U, void*, void* const, const void*, const void* const> ?
				sizeof(std::byte) : sizeof(std::remove_pointer_t<U>);

			return memory_overlaps<void*, void*>(
				access_as<void*>(start_1), size_1 * elm_size_1,
				access_as<void*>(start_2), size_2 * elm_size_2
			);
		}
	}

	/**
	 * @brief
	 *  Determines if the the memory region of <i>inner</i> is totally 
	 *  encompassed by that of <i>outer</i>.
	 */
	template<typename T, typename U = T>
	static constexpr bool memory_ecompases(const std::span<T>& outer, const std::span<U>& inner)
	{
		if (nullptr == outer.data() || nullptr == inner.data())
			return false;

		const std::byte* outer_begin = access_as<const std::byte*>(outer.data());
		const std::byte* outer_end = outer_begin + outer.size_bytes();

		const std::byte* inner_begin = access_as<const std::byte*>(inner.data());
		const std::byte* inner_end = inner_begin + inner.size_bytes();

		return (outer_begin <= inner_begin) && (outer_end >= inner_end);
	}



	/**
	 * @brief
	 *  Allocates memory that is properly aligned and sized for amount objects of type T.  No
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

	/**
	 * @brief
	 *  Returns the original value of ptr after setting the ptr reference passed to nullptr.
	 */
	template<typename T>
	static T* movePtr(T*& ptr)
	{
		T* ret = ptr;
		ptr = nullptr;

		return ret;
	}

	/**
	 * @brief
	 *  Calls std::destroy_at on _location_ but checks for a nullptr before doing so.
	 *  The passed pointer will be nullified.
	 */
	template<typename T>
	static void destruct_at(T* location)
	{
		if ( nullptr != location )
			std::destroy_at(location);
	}

	/**
	 * @brief
	 *  Moves the object at source to uninitialized memory at destination.
	 *  If T is move constructable, this will use placement move construction
	 *  at destination with source as the parameter, otherwise placement copy
	 *  construction will be used. The object at source, whether left invalid
	 *  from a move or a copy of what is now in destination, will be
	 *  destroyed after the operation.
	 *
	 * @details
	 *  Pre-Conditions
	 *  ------------------------
	 *  ------------------------
	 *  - <i>source</i> must be uninitialized memory.
	 *  - <i>source</i> must be be properly aligned for type T.
	 */
	template<typename T>
		requires CopyConstructable<T> || MoveConstructable<T>
	static void move_to(T* source, T* destination)
	{
		if constexpr ( MoveConstructable<T> )
			new(destination) T(std::move(*source));
		else if constexpr ( CopyConstructable<T> )
			new(destination) T(*source);

		std::destroy_at<T>(source);
	}

	template<typename ptr_t, typename contents_t, typename ...args>
	std::shared_ptr<ptr_t> make_dynamic_shared(args... params)
	{
		return std::dynamic_pointer_cast<ptr_t>(
			std::make_shared<contents_t>(std::forward<args>(params)...)
			);
	}
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // _STD_EXT_MEMORY_H_