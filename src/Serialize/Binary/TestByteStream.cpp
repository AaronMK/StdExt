#include <StdExt/Serialize/Binary/TestByteStream.h>
#include <StdExt/Serialize/Exceptions.h>

#include <algorithm>

namespace StdExt::Serialize::Binary
{
	TestByteStream::TestByteStream()
		: mSeekPosition(0), mMaxSeek(0)
	{
		setFlags(WRITE_ONLY | CAN_SEEK);
	}

	TestByteStream::~TestByteStream()
	{
	}

	void TestByteStream::writeRaw(const void* data, bytesize_t byteLength)
	{
		mSeekPosition += byteLength;
		mMaxSeek = std::max(mMaxSeek, mSeekPosition);
	}

	void TestByteStream::seek(seek_t position)
	{
		if (position < mMaxSeek)
			mSeekPosition = position;
		else
			throw OutOfBounds();
	}

	seek_t TestByteStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	bytesize_t TestByteStream::bytesAvailable() const
	{
		return 0;
	}

	bool TestByteStream::canRead(bytesize_t numBytes)
	{
		return false;
	}

	bool TestByteStream::canWrite(bytesize_t numBytes, bool autoExpand)
	{
		return true;
	}

	void TestByteStream::clear()
	{
		mSeekPosition = 0;
		mMaxSeek = 0;
	}
}