#ifndef _STD_EXT_MEMORY_SHARED_DATA_H_
#define _STD_EXT_MEMORY_SHARED_DATA_H_

#include "../Concepts.h"
#include "../Utility.h"

#include "Alignment.h"

#include <atomic>

namespace StdExt
{
	namespace Detail
	{
		template<typename metadata_t>
		class SharedBlockData
		{
		public:
			SharedBlockData() = default;
			virtual ~SharedBlockData() = default;

			using my_t = SharedBlockData<metadata_t>;
			
			mutable std::atomic<int> refCount = 1;
			void* dataPtr = nullptr;
			size_t size = 0;
			metadata_t metadata{};
			
			template<typename ...args_t>
			static my_t* make(size_t size, size_t alignment, args_t ...meta_args)
			{
				size_t max_align = std::max<size_t>(alignment, alignof(my_t));
				size_t padded_base_size = nextMutltipleOf(sizeof(my_t), alignment);

				void* mem = alloc_aligned(padded_base_size + size, max_align);
				my_t* result = new(mem) my_t(std::forward<args_t>(meta_args)...);

				result->dataPtr = (uint8_t*)mem + padded_base_size;
				result->size = size;

				return result;
			}

			static void free(my_t* block)
			{
				std::destroy_at<my_t>(block);
				free_aligned(block);
			}
		};

		template<>
		class SharedBlockData<void>
		{
		public:
			SharedBlockData() = default;
			virtual ~SharedBlockData() = default;

			using my_t = SharedBlockData<void>;

			mutable std::atomic<int> refCount = 1;
			void* dataPtr = nullptr;
			size_t size = 0;

			static my_t* make(size_t size, size_t alignment)
			{
				size_t max_align = std::max<size_t>(alignment, alignof(my_t));
				size_t padded_base_size = nextMutltipleOf(sizeof(my_t), alignment);

				void* mem = alloc_aligned(padded_base_size + size, max_align);
				my_t* result = new(mem) my_t();

				result->dataPtr = (uint8_t*)mem + padded_base_size;
				result->size = size;

				return result;
			}

			static void free(my_t* block)
			{
				std::destroy_at<my_t>(block);
				free_aligned(block);
			}
		};
	}

	/**
	 * @brief
	 *  Class to facilitate shared ownership of an arbitrary amount of raw data,
	 *  with optional metadata attached.  The reference count, optional metadata
	 *  structure, and raw data are contained in a single continuous allocation.
	 * 
	 * @tparam metadata_t
	 *  Data type of optional metadata to supplement the raw shared data.  Default void
	 *  type can be used to specify no metadata.
	 */
	template<DefaultConstructable metadata_t = void>
	class SharedData
	{
	private:
		using control_block_t = Detail::SharedBlockData<metadata_t>;
		control_block_t* mControlBlock;

		void release()
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
				control_block_t::free(mControlBlock);
						
			mControlBlock = nullptr;
		}

	public:

		/**
		 * @brief
		 *  Default constructor creates handle that reference no data.
		 */
		constexpr SharedData() noexcept
			: mControlBlock(nullptr)
		{
		}

		/**
		 * @brief
		 *  Move constructor will transfer any reference to data from
		 *  <i>other</i> to the constructed opbject.
		 */
		SharedData(SharedData&& other) noexcept
		{
			mControlBlock = other.mControlBlock;
			other.mControlBlock = nullptr;
		}

		/**
		 * @brief
		 *  Copy constructor adds a reference to the data of <i>other</i>
		 *  to this object and increments the reference count.
		 */
		SharedData(const SharedData& other) noexcept
			: SharedData()
		{
			control_block_t* otherBlock = other.mControlBlock;
		
			if (nullptr != otherBlock)
				++otherBlock->refCount;

			mControlBlock = otherBlock;
		}

		/**
		 * @brief
		 *  Constructs new shared data of the specified size and
		 *  alignment on the heap.
		 */
		SharedData(size_t size, size_t alignment = 1)
		: SharedData()
		{
			if (size == 0)
				return;

			mControlBlock = control_block_t::make(size, alignment);
		}

		/**
		 * @brief
		 *  Decrements and the reference count to shared data, freeing it
		 *  if it reaches zero.
		 */
		~SharedData()
		{
			release();
		}

		SharedData& operator=(const SharedData& other)
		{
			if ( other.mControlBlock != mControlBlock)
			{
				release();

				control_block_t* otherBlock = other.mControlBlock;

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
		
		/**
		 * @brief
		 *  Releases this object's reference to any shared data.
		 */
		void makeNull()
		{
			release();
		}

		/**
		 * @brief
		 *  Returns true if this object does not reference any data.
		 */
		bool isNull() const
		{
			return (nullptr == mControlBlock);
		}

		/**
		 * @brief
		 *  Gets the size of the referenced data in bytes, not including any
		 *  metadata.
		 */
		size_t size() const
		{
			return (mControlBlock) ? mControlBlock->size : 0;
		}

		/**
		 * @brief
		 *  Gets a pointer to the shared data if this object references any,
		 *  or returns nullptr if it does not.
		 */
		void* data()
		{
			return (mControlBlock) ? mControlBlock->dataPtr : nullptr;
		}
		
		/**
		 * @brief
		 *  Gets a const pointer to the shared data if this object references any,
		 *  or returns nullptr if it does not.
		 */
		void const* data() const
		{
			return (mControlBlock) ? mControlBlock->dataPtr : nullptr;
		}

		/**
		 * @brief
		 *  Gets a pointer to the metadata structure.  If this object does not
		 *  reference any shared data, or no metadata is specified, nullptr
		 *  is returned.
		 */
		metadata_t* metadata()
			requires (!std::same_as<void, metadata_t>)
		{
			return (mControlBlock) ? &mControlBlock->metadata : nullptr;
		}

		/**
		 * @brief
		 *  Gets a const pointer to the metadata structure.  If this object does not
		 *  reference any shared data, or no metadata is specified, nullptr
		 *  is returned.
		 */
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
}

#endif // !_STD_EXT_MEMORY_SHARED_DATA_H_