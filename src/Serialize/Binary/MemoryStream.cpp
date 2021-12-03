#include <StdExt/Serialize/Binary/MemoryStream.h>
#include <StdExt/Serialize/Exceptions.h>

using namespace std;

namespace StdExt::Serialize::Binary
{
	MemoryStream::MemoryStream()
		: ByteStream()
	{
		setFlags(READ_ONLY | MEMORY_BACKED | CAN_SEEK);

		mData = nullptr;
		mSize = 0;
		mSeekPosition = 0;
	}

	MemoryStream::MemoryStream(const void* beginning, bytesize_t size)
	{
		if (nullptr == beginning)
			throw std::invalid_argument("Parameter beginning cannot be null.");

		setFlags(MEMORY_BACKED | CAN_SEEK);

		mData = const_cast<void*>(beginning);
		mSize = (nullptr != mData) ? size : 0;
		mSeekPosition = 0;
	}

	MemoryStream::MemoryStream(void * beginning, bytesize_t size, Flags flags)
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

	void* MemoryStream::dataPtr(seek_t seekPos) const
	{
		if (nullptr == mData)
			throw InvalidOperation("Attempting to access an uninitialized MemoryStream.");

		if (seekPos >= mSize)
			throw OutOfBounds("Attempting to seek outside the bounds of a MemoryStream.");

		return (char*)mData + seekPos;
	}

	void MemoryStream::readRaw(void* destination, bytesize_t byteLength)
	{
		if (nullptr == mData)
			throw InvalidOperation("Attempting to read on an uninitialized MemoryStream.");
		
		if ((mSeekPosition + byteLength) <= mSize)
		{
			if (destination)
				memcpy(destination, &((char*)mData)[mSeekPosition], byteLength);
			
			mSeekPosition += byteLength;
		}

		throw OutOfBounds("Attempted to read passed the end of the MemoryStream.");
	}

	void MemoryStream::writeRaw(const void* data, bytesize_t byteLength)
	{
		if (nullptr == mData)
			throw InvalidOperation("Attempting to write on an uninitialized MemoryStream.");

		if ((getFlags() & READ_ONLY) != 0)
			throw InvalidOperation("Attempting to write on a readonly stream.");


		if ((mSeekPosition + byteLength) > mSize)
			throw OutOfBounds("Attempted to write passed the end of the MemoryStream.");

		memcpy(&((char*)mData)[mSeekPosition], data, byteLength);
		mSeekPosition += byteLength;
	}

	void MemoryStream::seek(seek_t position)
	{
		if (nullptr == mData)
			throw InvalidOperation("Attempting to seek on an uninitialized MemoryrStream.");

		if (position >= mSize)
			throw OutOfBounds("Attempted to sek passed the end of the MemoryStream.");

		mSeekPosition = position;
	}
	
	seek_t MemoryStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	bytesize_t MemoryStream::bytesAvailable() const
	{
		bytesize_t ret = mSize - mSeekPosition;
		return ret;
	}

	bool MemoryStream::canRead(bytesize_t numBytes)
	{
		if (nullptr == mData)
			return false;
		
		return ((mSeekPosition + numBytes) <= mSize);
	}

	bool MemoryStream::canWrite(bytesize_t numBytes, bool autoExpand)
	{
		if (nullptr == mData)
			return false;

		return ((getFlags() | READ_ONLY) == 0) ? ((mSeekPosition + numBytes) <= mSize) : false;
	}

	void MemoryStream::clear()
	{
		if ((getFlags() | READ_ONLY) != 0)
			throw InvalidOperation("Attepmted to clear a read-only stream.");

		seek(0);
	}
}