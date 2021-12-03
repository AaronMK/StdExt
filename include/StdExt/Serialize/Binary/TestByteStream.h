#ifndef _STD_EXT_SERIALIZE_TEST_BYTE_STREAM_H_
#define _STD_EXT_SERIALIZE_TEST_BYTE_STREAM_H_

#include "ByteStream.h"

namespace StdExt::Serialize::Binary
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

		virtual void writeRaw(const void* data, bytesize_t byteLength) override;
		virtual void seek(seek_t position) override;
		virtual seek_t getSeekPosition() const override;
		virtual bytesize_t bytesAvailable() const override;
		virtual bool canRead(bytesize_t numBytes) override;
		virtual bool canWrite(bytesize_t numBytes, bool autoExpand = false) override;
		virtual void clear() override;

	private:
		seek_t mSeekPosition;
		seek_t mMaxSeek;
	};
}

#endif // _STD_EXT_SERIALIZE_TEST_BYTE_STREAM_H_