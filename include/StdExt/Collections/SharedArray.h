#ifndef _STD_EXT_COLLECTIONS_SHARED_ARRAY_H_
#define _STD_EXT_COLLECTIONS_SHARED_ARRAY_H_

#include "Collections.h"

#include <cassert>

#if defined(STD_EXT_DEBUG)
#	include <span>
#endif

namespace StdExt::Collections
{
	template<typename T>
	class SharedArray final
	{

	#if defined(STD_EXT_DEBUG)
	using block_view_t = std::conditional_t< 
		Character<T>, std::basic_string_view<T>, std::span<T>
	>;
	#endif

	private:
		struct ControlBlock
		{
		#if defined(STD_EXT_DEBUG)
			block_view_t view;
		#endif

			std::atomic<int> refCount = 1;
			size_t size = 0;
			alignas(T) char allocStart = 0;
		};

		mutable ControlBlock* mControlBlock = nullptr;

		static ControlBlock* incrementBlock(ControlBlock* block)
		{
			if (block)
				++block->refCount;

			return block;
		}

		void decrementBlock()
		{
			if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
			{
				destroy_n(span());
				free_aligned(mControlBlock);
			}

			mControlBlock = nullptr;
		}

	public:
		constexpr SharedArray() = default;

		template<typename ...args_t>
		SharedArray(size_t count, args_t ...arguments)
			: SharedArray()
		{
			if (count > 0)
			{
				constexpr auto offset = offsetof(ControlBlock, allocStart);

				auto allocation = alloc_aligned(
					offset + sizeof(T) * count, alignof(ControlBlock)
				);

				mControlBlock = new (allocation)ControlBlock;
				mControlBlock->size = count;

				#if defined(STD_EXT_DEBUG)
					mControlBlock->view = block_view_t(
						access_as<T*>(&mControlBlock->allocStart),
						mControlBlock->size
					);
				#endif

				fill_uninitialized_n(
					span(),
					std::forward<args_t>(arguments)...
				);
			}
		}

		SharedArray(const SharedArray& other)
		{
			mControlBlock = incrementBlock(other.mControlBlock);
		}

		SharedArray(SharedArray&& other)
		{
			mControlBlock = movePtr(other.mControlBlock);
		}

		~SharedArray()
		{
			decrementBlock();
		}

		SharedArray& operator=(const SharedArray& other)
		{
			decrementBlock();
			mControlBlock = incrementBlock(other.mControlBlock);

			return *this;
		}

		SharedArray& operator=(SharedArray&& other)
		{
			decrementBlock();

			mControlBlock = movePtr(other.mControlBlock);

			return *this;
		}

		size_t size() const
		{
			return (mControlBlock) ? mControlBlock->size : 0;
		}

		std::span<T> span()
		{
			return (mControlBlock) ?
				std::span<T>(
					access_as<T*>(&mControlBlock->allocStart),
					mControlBlock->size
				) :
				std::span<T>();
		}

		T* data()
		{
			return access_as<T*>(&mControlBlock->allocStart);
		}

		const T* data() const
		{
			return access_as<const T*>(&mControlBlock->allocStart);
		}

		std::span<const T> span() const
		{
			return (mControlBlock) ?
				std::span<const T>(
					access_as<const T*>(&mControlBlock->allocStart),
					mControlBlock->size
				) :
				std::span<const T>();
		}

		T& operator[](size_t index)
		{
			assert(mControlBlock && mControlBlock->size > 0);
			return access_as<T*>(&mControlBlock->allocStart)[index];
		}

		const T& operator[](size_t index) const
		{
			return access_as<const T*>(&mControlBlock->allocStart)[index];
		}

		bool operator==(const SharedArray& other) const
		{
			return (mControlBlock == other.mControlBlock);
		}

		bool operator!=(const SharedArray& other) const
		{
			return (mControlBlock != other.mControlBlock);
		}

		operator bool() const
		{
			return (nullptr != mControlBlock);
		}

		void makeNull() noexcept
		{
			decrementBlock();
		}

		bool isNull() const
		{
			return (nullptr == mControlBlock);
		}
	};
}

#endif // !_STD_EXT_COLLECTIONS_SHARED_ARRAY_H_