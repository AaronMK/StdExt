#include <StdExt/Memory.h>

namespace StdExt
{
	MemoryReference::MemoryReference(MemoryReference&& other) noexcept
	{
		mControlBlock = other.mControlBlock;
		other.mControlBlock = nullptr;
	}

	MemoryReference::MemoryReference(const MemoryReference& other) noexcept
	{
		ControlBlock* otherBlock = other.mControlBlock;
		
		if (nullptr != otherBlock)
			++otherBlock->refCount;

		if (nullptr != mControlBlock && --mControlBlock->refCount)
			free(mControlBlock);

		mControlBlock = otherBlock;
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
	}

	MemoryReference::~MemoryReference()
	{
		if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
			free(mControlBlock);
	}

	MemoryReference& MemoryReference::operator=(const MemoryReference& other) noexcept
	{
		ControlBlock* otherBlock = other.mControlBlock;

		if (nullptr != otherBlock)
			++otherBlock->refCount;

		if (nullptr != mControlBlock && --mControlBlock->refCount)
			free(mControlBlock);

		mControlBlock = otherBlock;

		return *this;
	}

	MemoryReference& MemoryReference::operator=(MemoryReference&& other) noexcept
	{
		mControlBlock = other.mControlBlock;
		other.mControlBlock = nullptr;

		return *this;
	}

	void MemoryReference::makeNull()
	{
		if (nullptr != mControlBlock && 0 == --mControlBlock->refCount)
			free(mControlBlock);

		mControlBlock = nullptr;
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
}