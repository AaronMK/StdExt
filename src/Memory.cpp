#include <StdExt/Memory.h>

namespace StdExt
{
	MemoryReference::MemoryReference() noexcept
	{
		store(nullptr);
	}

	MemoryReference::MemoryReference(MemoryReference&& other) noexcept
	{
		store(other.lock());
		other.store(nullptr);
	}

	MemoryReference::MemoryReference(const MemoryReference& other) noexcept
	{
		ControlBlock* otherBlock = other.lock();

		if (otherBlock)
			++otherBlock->refCount;

		other.store(otherBlock);
		store(otherBlock);
	}

	MemoryReference::MemoryReference(size_t size, size_t alignment)
		: MemoryReference()
	{
		if (size == 0)
			return;

		size_t allocSize = sizeof(ControlBlock) + size + alignment - 1;
		mControlBlock = new(malloc(allocSize))ControlBlock();

		void* alignedStart = (void*)&mControlBlock->allocStart;
		std::align(alignment, size, alignedStart, allocSize);

		mControlBlock->alignedStart = alignedStart;
		mControlBlock->size = size;

		store(mControlBlock);
	}

	MemoryReference::~MemoryReference()
	{
		ControlBlock* oldBlock = lock();

		if (oldBlock && 0 == --oldBlock->refCount)
			free(oldBlock);
	}

	MemoryReference& MemoryReference::operator=(const MemoryReference& other) noexcept
	{
		ControlBlock* oldBlock = lock();

		if (oldBlock && --oldBlock->refCount == 0)
			free(oldBlock);

		ControlBlock* nextBlock = other.lock();
		if (nextBlock)
			++nextBlock->refCount;

		other.store(nextBlock);
		store(nextBlock);

		return *this;
	}

	MemoryReference& MemoryReference::operator=(MemoryReference&& other) noexcept
	{
		ControlBlock* oldBlock = lock();

		if (oldBlock && --oldBlock->refCount == 0)
			free(oldBlock);

		store(other.lock());
		other.store(nullptr);

		return *this;
	}

	void MemoryReference::makeNull()
	{
	}

	size_t MemoryReference::size() const
	{
		return (mControlBlock) ? mControlBlock->size : 0;
	}

	void* MemoryReference::data()
	{
		return (mControlBlock) ? mControlBlock->alignedStart : nullptr;
	}

	const void* MemoryReference::data() const
	{
		return (mControlBlock) ? mControlBlock->alignedStart : nullptr;
	}

	bool MemoryReference::operator==(const MemoryReference& other) const
	{
		return (mControlBlock == other.mControlBlock);
	}

	MemoryReference::operator bool() const
	{
		return (nullptr != mControlBlock);
	}

	MemoryReference::ControlBlock* MemoryReference::lock() const
	{
		ControlBlock* blockPtr = mControlBlock;

		uint64_t expected = TaggedBlock::pack(blockPtr, false);
		uint64_t desired = TaggedBlock::pack(blockPtr, true);

		while (!std::atomic_compare_exchange_weak(&mTaggedPtr, &expected, desired))
		{
			blockPtr = TaggedBlock::ptr(expected);

			expected = TaggedBlock::pack(blockPtr, false);
			desired = TaggedBlock::pack(blockPtr, true);
		}

		return TaggedBlock::ptr(desired);
	}

	void MemoryReference::store(ControlBlock* nextVal) const
	{
		mControlBlock = nextVal;
		mTaggedPtr.store(TaggedBlock::pack(mControlBlock, false));
	}
}