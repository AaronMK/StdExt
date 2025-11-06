#ifndef _STD_EXT_COLLECTIONS_QUEUE_H_
#define _STD_EXT_COLLECTIONS_QUEUE_H_

#include "../Memory/Alignment.h"

#include <span>
#include <type_traits>

namespace StdExt::Collections
{
	/**
	 * @brief 
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
	template<typename T, size_t local_size = 4, bool auto_shrink = true, size_t block_size = 16>
	class Queue
	{
		static_assert(block_size > 0, "block_size must be greater than 0.");

		template<typename other_t, size_t other_local, bool other_auto_shrink, size_t other_block>
		friend class Queue;

	protected:
		using buffer_t = std::conditional_t <
			local_size == 0,
			std::monostate,
			AlignedStorage<T, local_size>
		>;

		std::span<T> mAllocatedSpan;

	public:
		constexpr Queue() = default;
	};
}

#endif // _STD_EXT_COLLECTIONS_QUEUE_H_