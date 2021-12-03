#ifndef _STD_EXT_SERIALIZE_TEST_BYTE_STREAM_H_
#define _STD_EXT_SERIALIZE_TEST_BYTE_STREAM_H_

#include "ByteStream.h"

namespace StdExt::Streams
{
	/**
	 * A byte stream that does not actually write anything.  It can be used to determine
	 * seek positions after a series of operations, or as simply a black hole.  Of course,
	 * read operations will fail.
	 */
	class STD_EXT_EXPORT TestByteStream : public ByteStream
	{
	public:
		TestByteStream();
		virtual ~TestByteStream();

		virtual void writeRaw(const void* data, size_t byteLength) override;
		virtual void seek(size_t position) override;
		virtual size_t getSeekPosition() const override;
		virtual size_t bytesAvailable() const override;
		virtual bool canRead(size_t numBytes) override;
		virtual bool canWrite(size_t numBytes, bool autoExpand = false) override;
		virtual void clear() override;

	private:
		size_t mSeekPosition;
		size_t mMaxSeek;
	};
}

#endif // _STD_EXT_SERIALIZE_TEST_BYTE_STREAM_H_