#ifndef _STD_EXT_MEMORY_TAGGED_PTR_H_
#define _STD_EXT_MEMORY_TAGGED_PTR_H_

#include "../Concepts.h"

namespace StdExt
{	
	/**
	 * @brief
	 *  A structure that encodes both a pointer and a tag in a single uint64_t, allowing for more
	 *  compact data structures in 64-bit applications.
	 *
	 * @details
	 *  The tagged pointer takes advantage of the fact that address space of current 64-bit
	 *  processors is actually 48-bits, allowing the remaing 16-bits available to be used for a
	 *  tag value.  This allows for more compact data structures and better cache performance at
	 *  the cost of a mask to isolate the pointer, and a mask and a bit-shift operation to isolate the
	 *  tag.
	 */
	template<typename tag_t, PointerType ptr_t>
		requires (sizeof(tag_t) <= 2)
	struct TaggedPtr
	{
	private:
		static constexpr uint64_t TAG_MASK = 0xFFFF000000000000;
		static constexpr uint64_t PTR_MASK = 0x0000FFFFFFFFFFFF;

		uint64_t mData;

		#ifdef STD_EXT_DEBUG
		ptr_t dbgPointer{};
		tag_t dbgTag{};
		#endif // STD_EXT_DEBUG


		static uint64_t packTag(tag_t _tag)
		{
			if constexpr (sizeof(tag_t) == 1)
			{
				return (uint64_t)(reinterpret_cast<const uint8_t&>(_tag)) << 56;
			}
			else
			{
				return (uint64_t)(reinterpret_cast<const uint16_t&>(_tag)) << 48;
			}
		}

		static tag_t unpackTag(const uint64_t& val)
		{
			if constexpr (sizeof(tag_t) == 1)
			{
				uint8_t shifted = (uint8_t)(val >> 56);
				return reinterpret_cast<const tag_t&>(shifted);
			}
			else
			{
				uint16_t shifted = (uint16_t)(val >> 48);
				return reinterpret_cast<const tag_t&>(shifted);
			}
		}

	public:

		TaggedPtr()
		{
			mData = 0;
		}

		TaggedPtr(tag_t _tag, ptr_t _ptr)
		{
			mData = packTag(_tag) | (uint64_t)_ptr;
		}

		void pack(tag_t _tag, ptr_t _ptr)
		{
			#ifdef STD_EXT_DEBUG
			dbgPointer = _ptr;
			dbgTag = _tag;
			#endif // STD_EXT_DEBUG

			mData = packTag(_tag) | (uint64_t)_ptr;
		}

		void setTag(tag_t _tag)
		{
			#ifdef STD_EXT_DEBUG
			dbgTag = _tag;
			#endif // STD_EXT_DEBUG

			mData = packTag(_tag) | (mData & PTR_MASK);
		}

		tag_t tag()
		{
			return unpackTag(mData);
		}

		const tag_t tag() const
		{
			return unpackTag(mData);
		}

		void setPtr(ptr_t _ptr)
		{
			#ifdef STD_EXT_DEBUG
			dbgPointer = _ptr;
			#endif // STD_EXT_DEBUG

			mData = (mData & TAG_MASK) | ((uint64_t)_ptr & PTR_MASK);
		}

		ptr_t ptr()
		{
			return (ptr_t)(mData & PTR_MASK);
		}

		const ptr_t ptr() const
		{
			return (ptr_t)(mData & PTR_MASK);
		}

		const ptr_t operator->() const
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