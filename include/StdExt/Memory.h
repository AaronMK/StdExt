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

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

namespace StdExt
{
	template<typename T, typename U>
	static T* align(U*& ptr, std::size_t* space)
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
	static T* align(U*& ptr, std::size_t space)
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
			sizeof(_This) + alignof(_This) - 1,
			AlignedBlockSize<_Rest...>::value);
	};

	template<typename... _types>
	constexpr size_t AlignedBlockSize_v = AlignedBlockSize<_types...>::value;

	/**
	 * @brief
	 *  Allocates size bytes of memory with the specified alignment. 
	 *  The memory must be deallocated by using freeAligned() to
	 *  avoid a memory leak.
	 */
	static void* allocAligned(size_t size, size_t alignment)
	{
		#ifdef _MSC_VER
			return (size > 0) ? _aligned_malloc(size, alignment) : nullptr;
		#else
			return (size > 0) ? aligned_alloc(alignment, size) : nullptr;
		#endif
	}

	/*
	 * @brief
	 *  Frees memory allocated by allocAligned.
	 */
	static void freeAligned(void* ptr)
	{
		#ifdef _MSC_VER
		if (nullptr != ptr)
			_aligned_free(ptr);
		#else
			throw not_implemented("freeAligned() needs to be implmented.");
		#endif
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
	static void move_to(T* source, T* destination)
	{
		if constexpr (std::is_move_constructible_v<T>)
			new(destination) T(std::move(*source));
		else if constexpr (std::is_copy_constructible_v<T>)
			new(destination) T(*source);
		else
			static_assert(false, "T must be move or copy constructable.");

		std::destroy_at<T>(source);
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
	template<typename tag_t, typename ptr_t>
	struct TaggedPtr
	{
		static_assert(Traits<ptr_t>::is_pointer, "ptr_t must be a pointer type.");
		static_assert(sizeof(tag_t) <= 2, "tag_t must be two bytes or less in size.");

	private:
		static constexpr uint64_t TAG_MASK = 0xFFFF000000000000;
		static constexpr uint64_t PTR_MASK = 0x0000FFFFFFFFFFFF;

		uint64_t mData;

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

		TaggedPtr(ptr_t _ptr, tag_t _tag)
		{
			mData = packTag(_tag) | (uint64_t)ptr;
		}

		void pack(tag_t _tag, ptr_t _ptr)
		{
			mData = packTag(_tag) | (uint64_t)ptr;
		}

		void setTag(tag_t _tag)
		{
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

		ptr_t operator[](size_t index)
		{
			return (ptr())[index];
		}

		const ptr_t operator[](size_t index) const
		{
			return (ptr())[index];
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

	/**
	 * @brief
	 *  A shared reference to a dynamically sized block of memory.
	 */
	class STD_EXT_EXPORT MemoryReference final
	{
	private:
		struct ControlBlock
		{
			std::atomic<int> refCount = 1;
			size_t size = 0;
			void* alignedStart = nullptr;
			char allocStart = 0;
		};

	public:
		constexpr MemoryReference() noexcept
			: mControlBlock(nullptr)
		{
		}

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
		mutable ControlBlock* mControlBlock = nullptr;
	};

	template<size_t max_stack_bytes = 128>
	class StackBuffer
	{
	private:
		size_t mSize;
		char mLocalBuffer[max_stack_bytes];
		void* mBufferBegin;

	public:
		StackBuffer(const StackBuffer&) = delete;
		StackBuffer(StackBuffer&&) = delete;

		StackBuffer& operator=(const StackBuffer&) = delete;
		StackBuffer& operator=(StackBuffer&&) = delete;

		StackBuffer(size_t capacity)
		{
			mSize = capacity;

			if (mSize > max_stack_bytes)
				mBufferBegin = malloc(capacity);
			else
				mBufferBegin = mLocalBuffer;
		}

		~StackBuffer()
		{
			if (mSize > max_stack_bytes)
				free(mBufferBegin);
		}

		size_t size() const
		{
			return mSize;
		}

		void* data()
		{
			return mBufferBegin;
		}
	};

	template<typename T, size_t max_stack_elements>
	class StackArray
	{
	private:
		static constexpr size_t stack_byte_size = max_stack_elements * sizeof(T);
		alignas(T) std::byte mLocalBuffer[stack_byte_size];

		T* mElements;
		size_t mSize;

	public:
		StackArray(const StackArray&) = delete;
		StackArray(StackArray&&) = delete;

		StackArray& operator=(const StackArray&) = delete;
		StackArray& operator=(StackArray&&) = delete;

		static_assert(std::is_default_constructible_v<T>, "T must be default constructable.");

		StackArray(size_t numElements)
		{
			mSize = numElements;

			if (mSize > max_stack_elements)
				mElements = new T[mSize];
			else
				mElements = new(mLocalBuffer) T[mSize];
		}

		~StackArray()
		{
			if (mSize > max_stack_elements)
				delete[] mElements;
			else
				std::destroy_n(mElements, mSize);
		}

		T& operator[](size_t index)
		{
			return mElements[index];
		}

		const T& operator[](size_t index) const
		{
			return mElements[index];
		}

		size_t size() const
		{
			return mSize;
		}
	};

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