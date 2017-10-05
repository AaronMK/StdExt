#include <StdExt/Buffer.h>
#include <StdExt/Utility.h>

#include <cstdlib>
#include <stdexcept>
#include <type_traits>

namespace StdExt
{
	Buffer::Buffer()
	{
		mSize = 0;
		mBuffer = nullptr;
		mAlignment = 1;
	}

	Buffer::Buffer(Buffer&& other)
	{
		mSize = other.mSize;
		other.mSize = 0;

		mBuffer = other.mBuffer;
		other.mBuffer = nullptr;

		mAlignment = other.mAlignment;
		other.mAlignment = 0;
	}

	Buffer::Buffer(size_t size, size_t alignment)
		: Buffer()
	{
		resize(size, alignment);
	}


	Buffer::~Buffer()
	{
		if (nullptr != mBuffer)
			_aligned_free(mBuffer);
	}

	size_t Buffer::size() const
	{
		return mSize;
	}

	void Buffer::resize(size_t size, size_t alignment)
	{
		if (0 == alignment)
			alignment = mAlignment;

		if (false == isPowerOf2(alignment))
			throw std::invalid_argument("'alignment' must be a power of 2.");


		if (nullptr != mBuffer)
		{
			if (0 == size)
			{
				_aligned_free(mBuffer);
				mBuffer = nullptr;
				mSize = 0;
			}
			else
			{
				mBuffer = _aligned_realloc(mBuffer, size, alignment);
				mAlignment = alignment;
				mSize = size;
			}
		}
		else if (0 != size)
		{
			mBuffer = _aligned_malloc(size, alignment);
			mAlignment = alignment;
			mSize = size;
		}
	}

	const void * Buffer::data() const
	{
		return mBuffer;
	}

	void * Buffer::data()
	{
		return mBuffer;
	}
}