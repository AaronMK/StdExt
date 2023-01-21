#ifndef _STD_EXT_MEMORY_H_
#define _STD_EXT_MEMORY_H_

#include "StdExt.h"

#include "Exceptions.h"

#ifdef _MSC_VER
#	include <stdlib.h>
#else
#	include <cstring>
#	include <malloc.h>
#endif

#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <atomic>
#include <span>
#include <bit>

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

namespace StdExt
{
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
	 * 
	 *  When non-void pointer types are passed, regions are internally calculated,
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
	 *  encompased by that of <i>outer</i>.
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
			free(ptr);
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
		auto old_size = malloc_usable_size(ptr);
		void* ret = alloc_aligned(size, alignment);
		memcpy(ret, ptr, std::min(old_size, size));
		return ret;
	#endif
	}

	/**
	 * @brief
	 *  Allocates memory that is properly alligned and sized for amount objects of type T.  No
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

	template<Scaler T>
	T swap_endianness(T value)
	{
		if constexpr ( sizeof(T) == 1 )
		{
			return value;
		}
		else if constexpr ( sizeof(T) == 2 )
		{
			uint16_t op_val = access_as<uint16_t&>(&value);

			#ifdef _WIN32
				op_val = _byteswap_ushort(op_val);
			#else
				op_val = __builtin_bswap16(op_val);
			#endif
			
			return access_as<T&>(&op_val);
		}
		else if constexpr ( sizeof(T) == 4 )
		{
			uint32_t op_val = access_as<uint32_t&>(&value);

			#ifdef _WIN32
				op_val = _byteswap_ulong(op_val);
			#else
				op_val = __builtin_bswap32(op_val);
			#endif
			
			return access_as<T&>(&op_val);
		}
		else if constexpr ( sizeof(T) == 8 )
		{
			uint64_t op_val = access_as<uint64_t&>(&value);

			#ifdef _WIN32
				op_val = _byteswap_uint64(op_val);
			#else
				op_val = __builtin_bswap64(op_val);
			#endif
			
			return access_as<T&>(&op_val);
		}
	}

	/**
	 * @brief
	 *  Converts from the native byte order to big endian.
	 */
	template<Scaler T>
	static T to_big_endian(T value)
	{
		if constexpr ( std::endian::native == std::endian::little )
			return StdExt::swap_endianness(value);
		else
			return value;
	}
	
	/**
	 * @brief
	 *  Converts from the native byte order to little endian.
	 */
	template<Scaler T>
	static T to_little_endian(T value)
	{
		if constexpr ( std::endian::native == std::endian::big )
			return StdExt::swap_endianness(value);
		else
			return value;
	}

	/**
	 * @brief
	 *  Converts from big endian to the native byte order.
	 */
	template<Scaler T>
	static T from_big_endian(T value)
	{
		if constexpr ( std::endian::native == std::endian::little )
			return StdExt::swap_endianness(value);
		else
			return value;
	}
	
	/**
	 * @brief
	 *  Converts from little endian the native byte order.
	 */
	template<Scaler T>
	static T from_little_endian(T value)
	{
		if constexpr ( std::endian::native == std::endian::big )
			return StdExt::swap_endianness(value);
		else
			return value;
	}

	template<typename ptr_t, typename contents_t, typename ...args>
	std::shared_ptr<ptr_t> make_dynamic_shared(args... params)
	{
		return std::dynamic_pointer_cast<ptr_t>(
			std::make_shared<contents_t>(std::forward<args>(params)...)
			);
	}
	
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

	namespace Internal
	{
		struct ControlBlockBase
		{
			std::atomic<int> refCount = 1;

			virtual ~ControlBlockBase() = default;
			virtual void* ptr() = 0;
		};

		template<Polymorphic T>
		struct ControlBlock : public ControlBlockBase
		{
			T item;

			template<typename ...args_t>
			ControlBlock(args_t ...arguments)
				: item(std::forward<args_t>(arguments)...)
			{
			}

			virtual ~ControlBlock() = default;

			void* ptr() override
			{
				return &item;
			};
		};
	}

	template<StrippedType T>
	class SharedPtr;

	/**
	 * @brief
	 *  SharedPointer type that takes the same local space as a regular
	 *  pointer, but has some trade offs compared with std::shared_ptr.
	 *
	 *  Pros:
	 *   - Pointer object is the size of a single pointer.
	 *   - Managed object and control structure are always a single allocation.
	 *
	 *  Cons:
	 *   - For polymorphic types, dereferencing uses a virtual function call.
	 *
	 * @note
	 *  The SharedPtr<T> object itself is not thread safe, but the object it manages
	 *  maintains its thread safety charateristics.  Different SharedPtr<T> objects
	 *  referencing the same control structure reference count in a thread safe way.
	 */
	template<StrippedType T>
	class SharedPtr final
	{
		template<StrippedType U>
		friend class SharedPtr;

	private:

		struct BasicControlBlock
		{
			T item;
			std::atomic<int> refCount = 1;

			template<typename ...args_t>
			BasicControlBlock(args_t ...arguments)
				: item(std::forward<args_t>(arguments)...)
			{
			}

			inline void* ptr()
			{
				return &item;
			}
		};

		using block_ptr_t = std::conditional_t< Polymorphic<T>,
			Internal::ControlBlockBase*, BasicControlBlock* >;

		mutable block_ptr_t mControlBlock{};

		void incrementBlock() const
		{
			if (mControlBlock)
				++mControlBlock->refCount;
		}

		void decrementBlock() const
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
				delete mControlBlock;

			mControlBlock = nullptr;
		}

		template<typename U>
		inline void checkCast(const SharedPtr<U>& other)
		{
			static_assert(
				(InHeirarchyOf<T, U> && Polymorphic<T> && Polymorphic<U>) || Is<T, U>,
				"Casting between different SharedPtr types must be between polymorphic types "
				"in the same class hierachy of each other."
			);

			if constexpr (SubclassOf<T, U> && !Is<T, U>)
			{
				if (nullptr == other.mControlBlock)
					return;

				U* testPtr = access_as<U*>(other.mControlBlock->ptr());
				if (nullptr == dynamic_cast<T*>(testPtr))
					throw std::bad_cast();
			}
		}

	public:
		SharedPtr()
		{
		}

		template<typename U>
		SharedPtr(SharedPtr<U>&& other)
		{
			checkCast(other);

			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;
		}

		template<typename U>
		SharedPtr(const SharedPtr<U>& other)
		{
			checkCast(other);

			other.incrementBlock();
			mControlBlock = other.mControlBlock;
		}

		~SharedPtr()
		{
			decrementBlock();
		}

		template<typename U>
		SharedPtr& operator=(SharedPtr<U>&& other)
		{
			checkCast(other);
			decrementBlock();

			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;

			return *this;
		}

		template<typename U>
		SharedPtr& operator=(const SharedPtr<U>& other)
		{
			checkCast(other);
			decrementBlock();

			other.incrementBlock();
			mControlBlock = other.mControlBlock;

			return *this;
		}

		T* get()
		{
			return (mControlBlock) ?
				access_as<T*>(mControlBlock->ptr()) :
				nullptr;
		}

		const T* get() const
		{
			return (mControlBlock) ?
				access_as<const T*>(mControlBlock->ptr()) :
				nullptr;
		}

		T* operator->()
		{
			return get();
		}

		const T* operator->() const
		{
			return get();
		}

		T& operator*()
		{
			if (mControlBlock)
				return access_as<T&>(mControlBlock->ptr());

			throw null_pointer("Attempting to dereference a null pointer.");
		}

		const T& operator*() const
		{
			if (mControlBlock)
				return access_as<const T&>(mControlBlock->ptr());

			throw null_pointer();
		}

		operator bool() const
		{
			return (nullptr != mControlBlock);
		}

		void clear()
		{
			decrementBlock();
		}

		template<typename ...args_t>
		static SharedPtr make(args_t ...arguments)
		{
			SharedPtr ret;
			
			if constexpr ( Polymorphic<T> )
			{
				ret.mControlBlock = new Internal::ControlBlock<T>(std::forward<args_t>(arguments)...);
			}
			else
			{
				ret.mControlBlock = new BasicControlBlock(std::forward<args_t>(arguments)...);
			}

			return ret;
		}
	};

	/**
	 * @brief
	 *  A type that facilitates reference counted management of arbitrary data.  The
	 *  reference count, aligned data, and optional metadata are contained in a
	 *  single allocation.
	 */
	template<typename metadata_t = void>
	class SharedData
	{
		static_assert(
			std::same_as<void, metadata_t> || std::is_default_constructible_v<metadata_t>,
			"metadata_t must either be void or default constructable."
		);

	private:
		template<typename T>
		struct ControlBlock
		{
			mutable std::atomic<int> refCount = 1;
			size_t size = 0;
			metadata_t metadata{};
			void* alignedStart = nullptr;
			char allocStart = 0;
		};

		template<>
		struct ControlBlock<void>
		{
			mutable std::atomic<int> refCount = 1;
			size_t size = 0;
			void* alignedStart = nullptr;
			char allocStart = 0;
		};

		using control_block_t = ControlBlock<metadata_t>;

		control_block_t* mControlBlock;

		void release()
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
			{
				std::destroy_at<control_block_t>(mControlBlock);
				free(mControlBlock);
			}
		}

	public:
		constexpr SharedData() noexcept
			: mControlBlock(nullptr)
		{
		}

		SharedData(SharedData&& other) noexcept
		{
			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;
		}

		SharedData(const SharedData& other) noexcept
			: SharedData()
		{
			ControlBlock* otherBlock = other.mControlBlock;
		
			if (nullptr != otherBlock)
				++otherBlock->refCount;

			mControlBlock = otherBlock;
		}

		SharedData(size_t size, size_t alignment)
		: SharedData()
		{
			if (size == 0)
				return;

			size_t allocSize = sizeof(control_block_t) + size + alignment - 1;
			mControlBlock = new(malloc(allocSize))ControlBlock();

			void* alignedStart = (void*)&mControlBlock->allocStart;
			std::align(alignment, size, alignedStart, allocSize);

			mControlBlock->alignedStart = alignedStart;
			mControlBlock->size = size;
		}

		~SharedData()
		{
			release();
		}

		SharedData& operator=(const SharedData& other)
		{
			if ( other.mControlBlock != mControlBlock)
			{
				release();

				ControlBlock* otherBlock = other.mControlBlock;

				if (nullptr != otherBlock)
					++otherBlock->refCount;

				mControlBlock = otherBlock;
			}

			return *this;
		}

		SharedData& operator=(SharedData&& other)
		{
			release();
			
			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;

			return *this;
		}

		bool operator==(const SharedData& other) const
		{
			return (mControlBlock == other.mControlBlock);
		}

		void makeNull()
		{
			release();
			mControlBlock = nullptr;
		}

		bool isNull() const
		{
			return (nullptr == mControlBlock);
		}

		size_t size() const
		{
			return (mControlBlock) ? mControlBlock->size : 0;
		}

		void* data()
		{
			return (mControlBlock) ? mControlBlock->alignedStart : nullptr;
		}

		void const* data() const
		{
			return (mControlBlock) ? mControlBlock->alignedStart : nullptr;
		}

		metadata_t* metadata()
			requires (!std::same_as<void, metadata_t>)
		{
			return (mControlBlock) ? &mControlBlock->metadata : nullptr;
		}

		const metadata_t* metadata() const
			requires (!std::same_as<void, metadata_t>)
		{
			return (mControlBlock) ? &mControlBlock->metadata : nullptr;
		}

		operator bool() const
		{
			return (nullptr != mControlBlock);
		}
	};

	using MemoryReference = SharedData<void>;
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // _STD_EXT_MEMORY_H_