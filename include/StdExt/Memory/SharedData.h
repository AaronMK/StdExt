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
			size_t size;
			metadata_t metadata;
			
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
			size_t size;

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

	template<Defaultable metadata_t = void>
	class SharedData
	{
	private:
		using control_block_t = Detail::SharedBlockData<metadata_t>;
		control_block_t* mControlBlock;

		void release()
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
			{
				control_block_t::free(mControlBlock);
				mControlBlock = nullptr;
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
			control_block_t* otherBlock = other.mControlBlock;
		
			if (nullptr != otherBlock)
				++otherBlock->refCount;

			mControlBlock = otherBlock;
		}

		SharedData(size_t size, size_t alignment = 1)
		: SharedData()
		{
			if (size == 0)
				return;

			mControlBlock = control_block_t::make(size, alignment);
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

		void makeNull()
		{
			release();
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
			return (mControlBlock) ? mControlBlock->dataPtr : nullptr;
		}

		void const* data() const
		{
			return (mControlBlock) ? mControlBlock->dataPtr : nullptr;
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
}

#endif // !_STD_EXT_MEMORY_SHARED_DATA_H_