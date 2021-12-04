#include <StdExt/Streams/TestByteStream.h>
#include <StdExt/Exceptions.h>

#include <algorithm>

using namespace std;

namespace StdExt::Streams
{
	TestByteStream::TestByteStream()
		: mSeekPosition(0), mMaxSeek(0)
	{
		setFlags(WRITE_ONLY | CAN_SEEK);
	}

	TestByteStream::~TestByteStream()
	{
	}

	void TestByteStream::writeRaw(const void* data, size_t byteLength)
	{
		mSeekPosition += byteLength;
		mMaxSeek = std::max(mMaxSeek, mSeekPosition);
	}

	void TestByteStream::seek(size_t position)
	{
		if (position < mMaxSeek)
			mSeekPosition = position;
		else
			throw out_of_range("Attempted to seek outside the range of the stream.");
	}

	size_t TestByteStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	size_t TestByteStream::bytesAvailable() const
	{
		return 0;
	}

	bool TestByteStream::canRead(size_t numBytes)
	{
		return false;
	}

	bool TestByteStream::canWrite(size_t numBytes, bool autoExpand)
	{
		return true;
	}

	void TestByteStream::clear()
	{
		mSeekPosition = 0;
		mMaxSeek = 0;
	}
}