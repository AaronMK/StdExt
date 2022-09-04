#include <StdExt/Streams/ByteStream.h>

#include <StdExt/Exceptions.h>

#include <algorithm>
#include <assert.h>

using namespace std;

namespace StdExt::Streams

{
	ByteStream::ByteStream()
	{
		mFlags = INVALID;
	}

	ByteStream::~ByteStream()
	{

	}

	bool ByteStream::isValid() const
	{
		return ((mFlags & INVALID) == 0);
	}

	void* ByteStream::dataPtr(size_t seekPos) const
	{
		throw not_supported("Stream does support direct addressing of data.");
	}

	void ByteStream::skip(size_t byteLength)
	{
		try
		{
			seek(getSeekPosition() + byteLength);
		}
		catch (invalid_operation)
		{
			constexpr size_t BufferSize = 256;
			
			size_t BytesRemaining = byteLength;
			char Buffer[BufferSize];
			
			while (BytesRemaining > 0)
			{
				size_t ReadAmt = std::min(BufferSize, BytesRemaining);
				readRaw(Buffer, ReadAmt);

				BytesRemaining -= ReadAmt;
			}
		}
	}

	void ByteStream::readRaw(void* destination, size_t byteLength)
	{
		throw not_supported("Stream does not support reading.");
	}

	void ByteStream::writeRaw(const void* data, size_t byteLength)
	{
		throw not_supported("Stream does not support writing.");
	}

	void ByteStream::seek(size_t position)
	{
		throw not_supported("Stream does not seeking.");
	}

	size_t ByteStream::getSeekPosition() const
	{
		throw not_supported("Stream does not support seeking.");
	}

	void ByteStream::clear()
	{
		throw not_supported("Stream does not support clearing.");
	}

	uint32_t ByteStream::getFlags() const
	{
		return mFlags;
	}

	void ByteStream::setFlags(uint32_t mask)
	{
		mFlags = mask;
	}
}