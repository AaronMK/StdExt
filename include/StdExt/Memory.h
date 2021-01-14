#ifndef _STD_EXT_MEMORY_H_
#define _STD_EXT_MEMORY_H_

#include "StdExt.h"

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include "Exceptions.h"
#include "Concepts.h"
#include "Type.h"

#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <atomic>
#include <span>

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

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
	 *  of dest_size bytes and a byt alignment of dest_align.
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
	 *  Returns true if the passed regions of memory overlap. 
	 */
	constexpr bool memory_overlaps(void* start_1, size_t size_1, void* start_2, size_t size_2)
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
	
	/**
	 * @brief
	 *  Returns true if the passed regions of memory overlap. 
	 */
	template<typename T>
	constexpr bool memory_overlaps(std::span<T> region_1, std::span<T> region_2)
	{
		return memory_overlaps(
			region_1.data(), region_1.size() * sizeof(T),
			region_2.data(), region_2.size() * sizeof(T)
		);
	}

	/**
	 * @brief
	 *  Allocates size bytes of memory with the specified alignment. 
	 *  The memory must be deallocated by using free_aligned() to
	 *  avoid a memory leak.
	 */
	static void* alloc_aligned(size_t size, size_t alignment)
	{
		#ifdef _MSC_VER
			return (size > 0) ? _aligned_malloc(size, alignment) : nullptr;
		#else
			return (size > 0) ? aligned_alloc(alignment, size) : nullptr;
		#endif
	}

	/*
	 * @brief
	 *  Frees memory allocated by alloc_aligned.
	 */
	static void free_aligned(void* ptr)
	{
		#ifdef _MSC_VER
		if (nullptr != ptr)
			_aligned_free(ptr);
		#else
			throw not_implemented("free_aligned() needs to be implmented.");
		#endif
	}

	/*
	 * @brief
	 *  Reallocates and alligned allocation.  It is an error
	 *  to reallocate at a different allignment.
	 */
	static void* realloc_aligned(void* ptr, size_t size, size_t alignment)
	{
	#ifdef _MSC_VER
		if (nullptr != ptr)
			return _aligned_realloc(ptr, size, alignment);

		return nullptr;
	#else
		throw not_implemented("realloc_aligned() needs to be implmented.");
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
		bool isNull() const;

		size_t size() const;

		void* data();
		const void* data() const;

		/**
		 * @brief
		 *  Returns true if other is a reference to the same block as this reference.
		 */
		bool operator==(const MemoryReference& other) const;

		/**
		 * @brief
		 *  Returns true if this is not a null reference.
		 */
		operator bool() const;

	private:
		mutable ControlBlock* mControlBlock = nullptr;
	};

	template<typename ptr_t, typename contents_t, typename ...args>
	std::shared_ptr<ptr_t> make_dynamic_shared(args... params)
	{
		return std::dynamic_pointer_cast<ptr_t>(
			std::make_shared<contents_t>(std::forward<args>(params)...)
		);
	}

	/**
	 * @brief
	 *  Casts a pointer, taking care of any necessary constant or other casting needed
	 *  to force the conversion to work.
	 */
	template<PointerType out_t, PointerType in_t>
	out_t force_cast_pointer(in_t ptr)
	{
		return reinterpret_cast<out_t>(
			const_cast<void*>(
				reinterpret_cast<const void*>(ptr)
			)
		);
	}

	/**
	 * @brief
	 *  For debug configurations, this will perform a checked cast and throw errors
	 *  upon failure.  For release configurations, this will be a simple quick
	 *  unchecked cast.
	 */
	template<PointerType out_t, PointerType in_t>
	out_t cast_pointer(in_t ptr)
	{
	#ifdef STD_EXT_DEBUG
		out_t ret = dynamic_cast<out_t>(ptr);
		
		if (ret == nullptr && ptr != nullptr)
			throw std::bad_cast();

		return ret;
	#else
		return reinterpret_cast<out_t>(ptr);
	#endif
	}

	template<PointerType T, PointerType ptr_t>
	T access_as(ptr_t data)
	{
		return force_cast_pointer<T>(data);
	}

	template<ReferenceType T, PointerType ptr_t>
	T access_as(ptr_t data)
	{
		using cast_t = std::add_pointer_t<
			std::remove_reference_t<T>
		>;

		return *force_cast_pointer<cast_t>(data);
	}
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // _STD_EXT_MEMORY_H_