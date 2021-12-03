#include <StdExt/Serialize/Binary/ByteStream.h>
#include <StdExt/Serialize/Exceptions.h>

#include <algorithm>
#include <assert.h>


using namespace std;

namespace StdExt::Serialize::Binary

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

	void* ByteStream::dataPtr(seek_t seekPos) const
	{
		throw InvalidOperation("Stream does support direct addressing of data.");
	}

	void ByteStream::skip(bytesize_t byteLength)
	{
		try
		{
			seek(getSeekPosition() + byteLength);
		}
		catch (InvalidOperation)
		{
			constexpr bytesize_t BufferSize = 256;
			
			bytesize_t BytesRemaining = byteLength;
			char Buffer[BufferSize];
			
			while (BytesRemaining > 0)
			{
				bytesize_t ReadAmt = std::min(BufferSize, BytesRemaining);
				readRaw(Buffer, ReadAmt);

				BytesRemaining -= ReadAmt;
			}
		}
	}

	void ByteStream::readRaw(void* destination, bytesize_t byteLength)
	{
		throw InvalidOperation("Stream does not support reading.");
	}

	void ByteStream::writeRaw(const void* data, bytesize_t byteLength)
	{
		throw InvalidOperation("Stream does not support writing.");
	}

	void ByteStream::seek(seek_t position)
	{
		throw InvalidOperation("Stream does not seeking.");
	}

	seek_t ByteStream::getSeekPosition() const
	{
		throw InvalidOperation("Stream does not support seeking.");
	}

	void ByteStream::clear()
	{
		throw InvalidOperation("Stream does not support clearing.");
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