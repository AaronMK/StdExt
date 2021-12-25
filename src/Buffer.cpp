#include <StdExt/Buffer.h>
#include <StdExt/Utility.h>
#include <StdExt/Memory.h>

#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Streams/ByteStream.h>

#include <cstdlib>
#include <cstring>
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

	Buffer::Buffer(Buffer&& other) noexcept
	{
		mSize = other.mSize;
		other.mSize = 0;

		mBuffer = other.mBuffer;
		other.mBuffer = nullptr;

		mAlignment = other.mAlignment;
		other.mAlignment = 0;
	}

	Buffer::Buffer(const Buffer& other)
	{
		mBuffer = alloc_aligned(other.mSize, other.mAlignment);
		memcpy(mBuffer, other.mBuffer, other.mSize);

		mSize = other.mSize;
		mAlignment = other.mAlignment;
	}

	Buffer::Buffer(size_t size, size_t alignment)
		: Buffer()
	{
		resize(size, alignment);
	}


	Buffer::~Buffer()
	{
		if (nullptr != mBuffer)
			free_aligned(mBuffer);
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
				free_aligned(mBuffer);
				mBuffer = nullptr;
				mSize = 0;
			}
			else
			{
				mBuffer = realloc_aligned(mBuffer, size, alignment);
				mAlignment = alignment;
				mSize = size;
			}
		}
		else if (0 != size)
		{
			mBuffer = alloc_aligned(size, alignment);
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

	Buffer& Buffer::operator=(Buffer&& other)
	{
		if (nullptr != mBuffer)
			free_aligned(mBuffer);

		mSize = other.mSize;
		other.mSize = 0;

		mBuffer = other.mBuffer;
		other.mBuffer = nullptr;

		mAlignment = other.mAlignment;
		other.mAlignment = 0;

		return *this;
	}

	Buffer& Buffer::operator=(const Buffer& other)
	{
		if (nullptr != mBuffer)
		{
			free_aligned(mBuffer);

			mBuffer = nullptr;
			mSize = 0;
			mAlignment = 0;
		}

		if (mSize > 0)
		{
			mBuffer = alloc_aligned(other.mSize, other.mAlignment);
			memcpy(mBuffer, other.mBuffer, other.mSize);

			mSize = other.mSize;
			mAlignment = other.mAlignment;
		}

		return *this;
	}
}

namespace StdExt::Serialize::Binary
{
	template<>
	void read<StdExt::Buffer>(ByteStream* stream, StdExt::Buffer* out)
	{
		uint32_t length = read<uint32_t>(stream);

		StdExt::Buffer bufferOut(length);
		stream->readRaw(bufferOut.data(), length);

		(*out) = std::move(bufferOut);
	}

	template<>
	void write<StdExt::Buffer>(ByteStream* stream, const StdExt::Buffer& val)
	{
		uint32_t size = (uint32_t)val.size();

		write<uint32_t>(stream, size);
		stream->writeRaw(val.data(), size);
	}
}