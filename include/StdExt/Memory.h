#ifndef _STD_EXT_MEMORY_H_
#define _STD_EXT_MEMORY_H_

#include "StdExt.h"

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include "Exceptions.h"

#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <atomic>


namespace StdExt
{
	template<typename T, typename U>
	T* align(U*& ptr, std::size_t* space)
	{
		void* vPtr = ptr;

		if (std::align(alignof(T), sizeof(T), vPtr, *space))
		{
			ptr = reinterpret_cast<U*>(vPtr);
			return reinterpret_cast<T*>(vPtr);
		}

		return nullptr;
	}

	template<typename T, typename U>
	T* align(U*& ptr, std::size_t space)
	{
		return align<T>(ptr, &space);
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
			sizeof(_This) + alignof(_This)-1,
			AlignedBlockSize<_Rest...>::value);
	};

	template<typename... _types>
	constexpr size_t AlignedBlockSize_v = AlignedBlockSize<_types...>::value;

	/**
	 * @brief
	 *  Returns the original value of ptr after setting the ptr reference passed to nullptr.
	 */
	template<typename T>
	T* movePtr(T*& ptr)
	{
		T* ret = ptr;
		ptr = nullptr;

		return ret;
	}

	/**
	 * @brief
	 *   A structure that encodes both a pointer and a tag in a single uint64_t, allowing for more
	 *  compact data structures in 64-bit applications.
	 *  
	 * @details
	 *  The tagged pointer takes advantage of the fact that address space of current 64-bit
	 *  processors is actually 48-bits, allowing the remaing 16-bits available to be used for a
	 *  tag value.  This allows for more compact data structures and better cache performance at
	 *  the cost of a mask and bit-shift to isolate the pointer, and a mask operation to isolate the
	 *  tag.
	 */
	template<typename ptr_t, typename tag_t>
	struct TaggedPtr
	{
	private:
		uint64_t mData;

	public:
		static constexpr uint64_t TAG_MASK = 0x000000000000FFFF;
		static constexpr uint64_t PTR_MASK = 0xFFFFFFFFFFFF0000;

		static uint64_t pack(ptr_t* _ptr, tag_t _tag)
		{
			return (uint64_t(_tag) & TAG_MASK) | ( ((uint64_t)_ptr << 16) & PTR_MASK );
		}

		static tag_t tag(uint64_t _data)
		{
			return (tag_t)(_data & TAG_MASK);
		}

		static ptr_t* ptr(uint64_t _data)
		{
			return (ptr_t*)((_data & PTR_MASK) >> 16);
		}

		static uint64_t setPtr(uint64_t _data, const ptr_t* _ptr)
		{
			return (_data & TAG_MASK) | ( ((uint64_t)_ptr << 16) & PTR_MASK );
		}

		static uint64_t setTag(uint64_t _data, tag_t _tag)
		{
			return (_data & PTR_MASK) | (uint64_t(_tag) & TAG_MASK);
		}

		TaggedPtr<ptr_t, tag_t>(const TaggedPtr<ptr_t, tag_t>& other) = default;
		TaggedPtr<ptr_t, tag_t>& operator=(const TaggedPtr<ptr_t, tag_t>& other) = default;

		TaggedPtr()
		{
			mData = 0;
		}

		TaggedPtr(ptr_t* _ptr, tag_t _tag)
		{
			mData = pack(_ptr, _tag);
		}

		void setPtr(tag_t _tag)
		{
			mData = setTag(mData, _tag);
		}

		tag_t tag()
		{
			return tag(mData);
		}

		const tag_t tag() const
		{
			return tag(mData);
		}

		void setPtr(ptr_t* _ptr)
		{
			mData = setPtr(mData, _ptr);
		}

		ptr_t* ptr()
		{
			return ptr(mData);
		}

		const ptr_t* ptr() const
		{
			return ptr(mData);
		}
	};

	/**
	 * @brief
	 *  A shared reference to a dynamically sized block of memory.
	 */
	class MemoryReference final
	{
	private:
		struct ControlBlock
		{
			mutable std::atomic<int> refCount = 1;
			size_t size = 0;
			void* alignedStart = nullptr;
			char allocStart = 0;
		};

	public:
		MemoryReference() noexcept;
		MemoryReference(MemoryReference&& other) noexcept;
		MemoryReference(const MemoryReference& other) noexcept;
		MemoryReference(size_t size, size_t alignment = 1);

		~MemoryReference();

		MemoryReference& operator=(const MemoryReference& other) noexcept;
		MemoryReference& operator=(MemoryReference&& other) noexcept;

		void makeNull();

		size_t size() const;

		void* data();
		const void* data() const;

		bool operator==(const MemoryReference& other) const;
		operator bool() const;

	private:
		using TaggedBlock = TaggedPtr<ControlBlock, bool>;

		mutable std::atomic<uint64_t> mTaggedPtr;
		mutable ControlBlock* mControlBlock = nullptr;

		ControlBlock* lock() const;
		void store(ControlBlock* nextVal) const;
	};
}

#endif // _STD_EXT_MEMORY_H_