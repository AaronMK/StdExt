#ifndef _STD_EXT_MEMORY_ALIGNMENT_H_
#define _STD_EXT_MEMORY_ALIGNMENT_H_

#include "../Concepts.h"

namespace StdExt
{
	/**
	 * @brief
	 *  Aligns a pointer according to the alignment and size requirements
	 *  for a specific data type, if possible with the available space.
	 *
	 * @tparam for_t
	 *  Type to used to determine the alignment and size requirements.
	 *
	 * @param ptr
	 *  The pointer to be realigned.  The address when passed should be
	 *  the starting point of the buffer.
	 *
	 * @param space
	 *  A refrence to a variable containing the amount of available space.
	 *  If the function succeeds, its value will become the space remaining
	 *  after padding to align the pointer.
	 *
	 * @return
	 *  True if the pointer is successfully realigned.  If not successful,
	 *  the passed parameters will remain unchanged.
	 */
	template<StrippedType for_t, PointerType ptr_t>
	static bool align_for(ptr_t& ptr, size_t& space)
	{
		constexpr size_t size = sizeof(for_t);
		constexpr size_t alignment = alignof(for_t);
		
		void* vPtr = ptr;

		if (nullptr != std::align(alignment, size, vPtr, space))
		{
			ptr = reinterpret_cast<ptr_t>(vPtr);
			return true;
		}

		return false;
	}

	/**
	 * @brief
	 *  Returns true if data with a size of src_size and alignment requirement of src_align can be
	 *  always be placed in memory of dest_size and aligned with dest_align.  The logic assumes alignments are
	 *  powers of two.
	 */
	static constexpr bool can_place_aligned(size_t src_size, size_t src_align, size_t dest_size, size_t dest_align)
	{
		if ( src_size > dest_size )
			return false;

		src_align = std::max<size_t>(1, src_align);
		dest_align = std::max<size_t>(1, dest_align);

		size_t max_padding = (src_align > dest_align) ? (src_align - dest_align) : 0;
		return ( (src_size + max_padding) <= dest_size );
	}

	/**
	 * @brief
	 *  Returns true if an object of type T can always be placed in a memory block
	 *  of dest_size bytes and a byte alignment of dest_align.
	 */
	template<typename T>
	static constexpr bool can_place_aligned(size_t dest_size, size_t dest_align)
	{
		return can_place_aligned(sizeof(T), alignof(T), dest_size, dest_align);
	}

	template<typename... _Types>
	struct AlignedBlockSize;

	template<>
	struct AlignedBlockSize<>
	{
		static const size_t value = 1;
	};

	/**
	 * @brief
	 *  The value member of this struct contains the smallest block size
	 *  for which std::align would be guarenteed at succeeding for all
	 *  type parameters.
	 */
	template<typename _This, typename... _Rest>
	struct AlignedBlockSize<_This, _Rest...>
	{
		static constexpr size_t value = std::max(
			sizeof(_This) + alignof(_This) - 1,
			AlignedBlockSize<_Rest...>::value);
	};

	template<typename... _types>
	constexpr size_t AlignedBlockSize_v = AlignedBlockSize<_types...>::value;

    /**
	 * @brief
	 *  Allocates size bytes of memory with the specified alignment. 
	 *  The memory must be deallocated by using free_aligned() to
	 *  avoid a memory leak.
	 */
	extern void* alloc_aligned(size_t size, size_t alignment);

	/*
	 * @brief
	 *  Frees memory allocated by alloc_aligned.
	 */
	extern void free_aligned(void* ptr);

	/*
	 * @brief
	 *  Reallocates and alligned allocation.  It is an error
	 *  to reallocate at a different allignment.
	 */
	extern void* realloc_aligned(void* ptr, size_t size, size_t alignment);
}

#endif // !_STD_EXT_MEMORY_ALIGNMENT_H_