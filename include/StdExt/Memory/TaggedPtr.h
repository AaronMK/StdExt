#ifndef _STD_EXT_MEMORY_TAGGED_PTR_H_
#define _STD_EXT_MEMORY_TAGGED_PTR_H_

#include "../Concepts.h"

#include <bit>

namespace StdExt
{	
	/**
	 * @brief
	 *  A structure that encodes both a pointer and a tag in a single uint64_t, allowing for more
	 *  compact data structures in 64-bit applications.
	 *
	 * @details
	 *  The tagged pointer takes advantage of the fact that address space of current 64-bit
	 *  processors is actually 56-bits, allowing the remaining 8-bits available to be used for a
	 *  tag value.  This allows for more compact data structures and better cache performance at
	 *  the cost of a mask to isolate the pointer, and a mask and a bit-shift operation to isolate the
	 *  tag.
	 * 
	 * @note
	 *  This class may be removed.  Consistency in the most significant bits (and how many) not being used are
	 *  not what they used to be.
	 */
	template<typename tag_t, PointerType ptr_t>
		requires (sizeof(tag_t) == 1)
	struct TaggedPtr
	{
		static_assert(sizeof(ptr_t) == sizeof(uint64_t), "Only available for 64-bit pointers");

	private:
		static constexpr uint64_t TAG_MASK = 0xFF00000000000000;
		static constexpr uint64_t PTR_MASK = 0x00FFFFFFFFFFFFFF;

		uint64_t mData = 0;

		#ifdef STD_EXT_DEBUG
		ptr_t dbgPointer{};
		tag_t dbgTag{};
		#endif // STD_EXT_DEBUG

		static uint64_t packTag(tag_t _tag)
		{
			return static_cast<uint64_t>(std::bit_cast<uint8_t>(_tag)) << 56;
		}

		static tag_t unpackTag(uint64_t val)
		{
			return std::bit_cast<tag_t>(static_cast<uint8_t>(val >> 56));
		}

	public:
		using const_ptr_t = const std::remove_pointer_t<ptr_t>*;

		constexpr TaggedPtr() = default;

		TaggedPtr(tag_t _tag, ptr_t _ptr)
		{
			pack(_tag, _ptr);
		}

		void pack(tag_t _tag, ptr_t _ptr)
		{
			#ifdef STD_EXT_DEBUG
			dbgPointer = _ptr;
			dbgTag = _tag;
			#endif // STD_EXT_DEBUG

			mData = packTag(_tag) | (std::bit_cast<uint64_t>(_ptr) & PTR_MASK);
		}

		void setTag(tag_t _tag)
		{
			#ifdef STD_EXT_DEBUG
			dbgTag = _tag;
			#endif // STD_EXT_DEBUG

			mData = packTag(_tag) | (mData & PTR_MASK);
		}

		tag_t tag() const
		{
			return unpackTag(mData);
		}

		void setPtr(ptr_t _ptr)
		{
			#ifdef STD_EXT_DEBUG
			dbgPointer = _ptr;
			#endif // STD_EXT_DEBUG

			mData = (mData & TAG_MASK) | (std::bit_cast<uint64_t>(_ptr) & PTR_MASK);
		}

		ptr_t ptr()
		{
			return std::bit_cast<ptr_t>(mData & PTR_MASK);
		}

		const_ptr_t ptr() const
		{
			return std::bit_cast<const_ptr_t>(mData & PTR_MASK);
		}

		const_ptr_t operator->() const
		{
			return ptr();
		}

		ptr_t operator->()
		{
			return ptr();
		}
	};
}

#endif // !_STD_EXT_MEMORY_TAGGED_PTR_H_