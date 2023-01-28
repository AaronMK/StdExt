#include <StdExt/Streams/SocketStream.h>

#include <StdExt/Exceptions.h>
#include <StdExt/Memory/Casting.h>

namespace StdExt::Streams
{
	static constexpr size_t BLOCK_SIZE = 256;
	
	SocketStream::SocketStream()
	{
		setFlags(MEMORY_BACKED);

		mReadMarker = 0;
		mWriteMarker = 0;
	}

	SocketStream::~SocketStream() = default;

	SocketStream::SocketStream(SocketStream&&) = default;
	SocketStream& SocketStream::operator=(SocketStream&&) = default;

	void* SocketStream::dataPtr(size_t seekPos) const
	{
		if (0 == mWriteMarker - mReadMarker)
			return nullptr;

		std::byte* byte_ptr = access_as<std::byte*>(mBuffer.data());
		return &byte_ptr[mReadMarker];
	}
	
	void SocketStream::readRaw(void* destination, size_t byteLength)
	{
		if (0 == byteLength)
			return;
		
		if (mWriteMarker - mReadMarker < byteLength)
			throw std::out_of_range("Not enough data on bytestream to complete read request.");

		std::byte* byte_ptr = access_as<std::byte*>(mBuffer.data());
		std::memcpy(destination, &byte_ptr[mReadMarker], byteLength);

		mReadMarker += byteLength;

		if ( mReadMarker == mWriteMarker )
		{
			mWriteMarker = 0;
			mReadMarker = 0;
		}
	}

	void SocketStream::writeRaw(const void* data, size_t byteLength)
	{
		std::memcpy(expandForWrite(byteLength), data, byteLength);
	}

	size_t SocketStream::bytesAvailable() const
	{
		return mWriteMarker - mReadMarker;
	}

	bool SocketStream::canRead(size_t numBytes)
	{
		return bytesAvailable() >= numBytes;
	}

	bool SocketStream::canWrite(size_t numBytes, bool autoExpand)
	{
		return true;
	}

	void* SocketStream::expandForWrite(size_t byteLength)
	{ 
		if (mWriteMarker + byteLength > mBuffer.size())
		{
			discardAlreadyRead();

			size_t size_needed = mWriteMarker + byteLength;

			if (mBuffer.size() < size_needed)
			{	
				size_t blocks_needed = (0 == size_needed % BLOCK_SIZE) ?
					size_needed / BLOCK_SIZE : (size_needed / BLOCK_SIZE + 1);

				size_t next_size = blocks_needed * BLOCK_SIZE;
				mBuffer.resize(next_size);
			}
		}

		void* write_start = access_as<std::byte*>(mBuffer.data()) + mWriteMarker;
		mWriteMarker += byteLength;

		return write_start;
	}
	
	void SocketStream::discardAlreadyRead()
	{
		size_t count = mWriteMarker - mReadMarker;

		if (mReadMarker > 0 && count > 0)
		{
			std::byte* byte_ptr = access_as<std::byte*>(mBuffer.data());
			std::memmove(byte_ptr, &byte_ptr[mReadMarker], count);
		}

		mWriteMarker -= mReadMarker;
		mReadMarker = 0;
	}

	void SocketStream::clear()
	{
		mWriteMarker = 0;
		mReadMarker = 0;
	}
}