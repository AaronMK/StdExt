#include <StdExt/Streams/MemoryStream.h>
#include <StdExt/Exceptions.h>

#include <stdexcept>

using namespace std;

namespace StdExt::Streams
{
	MemoryStream::MemoryStream()
		: ByteStream()
	{
		setFlags(READ_ONLY | MEMORY_BACKED | CAN_SEEK);

		mData = nullptr;
		mSize = 0;
		mSeekPosition = 0;
	}

	MemoryStream::MemoryStream(const void* beginning, size_t size)
	{
		if (nullptr == beginning)
			throw std::invalid_argument("Parameter beginning cannot be null.");

		setFlags(MEMORY_BACKED | CAN_SEEK);

		mData = const_cast<void*>(beginning);
		mSize = (nullptr != mData) ? size : 0;
		mSeekPosition = 0;
	}

	MemoryStream::MemoryStream(void * beginning, size_t size, Flags flags)
	{
		if (nullptr == beginning)
			throw std::invalid_argument("Parameter beginning cannot be null.");

		setFlags(MEMORY_BACKED | CAN_SEEK | flags);

		mData = beginning;
		mSize = (nullptr != mData) ? size : 0;
		mSeekPosition = 0;
	}

	MemoryStream::~MemoryStream()
	{
	}

	void* MemoryStream::dataPtr(size_t seekPos) const
	{
		if (nullptr == mData)
			throw invalid_operation("Attempting to access an uninitialized MemoryStream.");

		if (seekPos >= mSize)
			throw out_of_range("Attempting to seek outside the bounds of a MemoryStream.");

		return (char*)mData + seekPos;
	}

	void MemoryStream::readRaw(void* destination, size_t byteLength)
	{
		if (nullptr == mData)
			throw invalid_operation("Attempting to read on an uninitialized MemoryStream.");
		
		if ((mSeekPosition + byteLength) <= mSize)
		{
			if (destination)
				memcpy(destination, &((char*)mData)[mSeekPosition], byteLength);
			
			mSeekPosition += byteLength;
		}

		throw out_of_range("Attempted to read passed the end of the MemoryStream.");
	}

	void MemoryStream::writeRaw(const void* data, size_t byteLength)
	{
		if (nullptr == mData)
			throw invalid_operation("Attempting to write on an uninitialized MemoryStream.");

		if ((getFlags() & READ_ONLY) != 0)
			throw invalid_operation("Attempting to write on a readonly stream.");


		if ((mSeekPosition + byteLength) > mSize)
			throw out_of_range("Attempted to write passed the end of the MemoryStream.");

		memcpy(&((char*)mData)[mSeekPosition], data, byteLength);
		mSeekPosition += byteLength;
	}

	void MemoryStream::seek(size_t position)
	{
		if (nullptr == mData)
			throw invalid_operation("Attempting to seek on an uninitialized MemoryrStream.");

		if (position >= mSize)
			throw out_of_range("Attempted to sek passed the end of the MemoryStream.");

		mSeekPosition = position;
	}
	
	size_t MemoryStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	size_t MemoryStream::bytesAvailable() const
	{
		size_t ret = mSize - mSeekPosition;
		return ret;
	}

	bool MemoryStream::canRead(size_t numBytes)
	{
		if (nullptr == mData)
			return false;
		
		return ((mSeekPosition + numBytes) <= mSize);
	}

	bool MemoryStream::canWrite(size_t numBytes, bool autoExpand)
	{
		if (nullptr == mData)
			return false;

		return ((getFlags() | READ_ONLY) == 0) ? ((mSeekPosition + numBytes) <= mSize) : false;
	}

	void MemoryStream::clear()
	{
		if ((getFlags() | READ_ONLY) != 0)
			throw invalid_operation("Attepmted to clear a read-only stream.");

		seek(0);
	}
}