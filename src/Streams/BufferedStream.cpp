#include <StdExt/Streams/BufferedStream.h>

#include <stdexcept>

using namespace std;

namespace StdExt::Streams
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

	void* BufferedStream::dataPtr(size_t seekPos) const
	{
		if (seekPos >= mBytesWritten)
			throw out_of_range("Attempted to seek out of the rnage of the stream.");

		return ((char*)mBuffer.data() + seekPos);
	}

	void BufferedStream::readRaw(void* destination, size_t byteLength)
	{
		if (mSeekPosition + byteLength > mBytesWritten)
			throw out_of_range("Attempted to read beyond the range of the stream.");

		if (0 < byteLength)
		{
			memcpy(destination, dataPtr(mSeekPosition), byteLength);
			mSeekPosition += byteLength;
		}
	}

	void BufferedStream::writeRaw(const void* data, size_t byteLength)
	{
		if (mSeekPosition + byteLength >= mBuffer.size())
			mBuffer.resize((mSeekPosition + byteLength) / BLOCK_SIZE + BLOCK_SIZE);

		memcpy((char*)mBuffer.data() + mSeekPosition, data, byteLength);
		mSeekPosition += byteLength;

		if (mBytesWritten <= mSeekPosition)
			mBytesWritten = mSeekPosition;
	}

	void BufferedStream::seek(size_t position)
	{
		if (position >= mBytesWritten)
			throw out_of_range("Attempted to seek out of the rnage of the stream.");

		mSeekPosition = position;
	}

	size_t BufferedStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	size_t BufferedStream::bytesAvailable() const
	{
		return mBytesWritten - mSeekPosition - 1;
	}

	bool BufferedStream::canRead(size_t numBytes)
	{
		return (mSeekPosition + numBytes < mBytesWritten);
	}

	bool BufferedStream::canWrite(size_t numBytes, bool autoExpand)
	{
		return (autoExpand || (mSeekPosition + numBytes < mBuffer.size()));
	}

	void BufferedStream::clear()
	{
		mBuffer.resize(0);
		mSeekPosition = 0;
		mBytesWritten = 0;
	}

	void* BufferedStream::expandForWrite(size_t byteLength)
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