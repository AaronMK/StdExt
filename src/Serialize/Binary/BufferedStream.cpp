#include <StdExt/Serialize/Binary/BufferedStream.h>
#include <StdExt/Serialize/Exceptions.h>

namespace StdExt::Serialize::Binary
{
	static const size_t BLOCK_SIZE = 256;

	BufferedStream::BufferedStream()
	{
		setFlags(CAN_SEEK | MEMORY_BACKED);

		mBytesWritten = 0;
		mSeekPosition = 0;
	}
	
	BufferedStream::~BufferedStream()
	{
	}

	void* BufferedStream::dataPtr(seek_t seekPos) const
	{
		if (seekPos >= mBytesWritten)
			throw OutOfBounds();

		return ((char*)mBuffer.data() + seekPos);
	}

	void BufferedStream::readRaw(void* destination, bytesize_t byteLength)
	{
		if (mSeekPosition + byteLength > mBytesWritten)
			throw OutOfBounds();

		if (0 < byteLength)
		{
			memcpy(destination, dataPtr(mSeekPosition), byteLength);
			mSeekPosition += byteLength;
		}
	}

	void BufferedStream::writeRaw(const void* data, bytesize_t byteLength)
	{
		if (mSeekPosition + byteLength >= mBuffer.size())
			mBuffer.resize((mSeekPosition + byteLength) / BLOCK_SIZE + BLOCK_SIZE);

		memcpy((char*)mBuffer.data() + mSeekPosition, data, byteLength);
		mSeekPosition += byteLength;

		if (mBytesWritten <= mSeekPosition)
			mBytesWritten = mSeekPosition;
	}

	void BufferedStream::seek(seek_t position)
	{
		if (position >= mBytesWritten)
			throw OutOfBounds();

		mSeekPosition = position;
	}

	seek_t BufferedStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	bytesize_t BufferedStream::bytesAvailable() const
	{
		return mBytesWritten - mSeekPosition - 1;
	}

	bool BufferedStream::canRead(bytesize_t numBytes)
	{
		return (mSeekPosition + numBytes < mBytesWritten);
	}

	bool BufferedStream::canWrite(bytesize_t numBytes, bool autoExpand)
	{
		return (autoExpand || (mSeekPosition + numBytes < mBuffer.size()));
	}

	void BufferedStream::clear()
	{
		mBuffer.resize(0);
		mSeekPosition = 0;
		mBytesWritten = 0;
	}

	void* BufferedStream::expandForWrite(bytesize_t byteLength)
	{
		if (mSeekPosition + byteLength >= mBuffer.size())
			mBuffer.resize((mSeekPosition + byteLength) / BLOCK_SIZE + BLOCK_SIZE);
		
		void* ret = (char*)mBuffer.data() + mSeekPosition;
		mSeekPosition += byteLength;

		if (mBytesWritten <= mSeekPosition)
			mBytesWritten = mSeekPosition;

		return ret;
	}
}