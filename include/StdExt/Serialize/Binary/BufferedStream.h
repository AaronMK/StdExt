#ifndef _STD_EXT_SERIALIZE_BUFFERED_STREAM_H_
#define _STD_EXT_SERIALIZE_BUFFERED_STREAM_H_

#include "ByteStream.h"

#include "../../Buffer.h"

namespace StdExt::Serialize::Binary
{
	class STD_EXT_EXPORT BufferedStream : public ByteStream
	{
	public:
		BufferedStream();
		virtual ~BufferedStream();

		virtual void* dataPtr(seek_t seekPos) const override;
		virtual void readRaw(void* destination, bytesize_t byteLength) override;
		virtual void writeRaw(const void* data, bytesize_t byteLength) override;
		virtual void seek(seek_t position) override;
		virtual seek_t getSeekPosition() const override;
		virtual bytesize_t bytesAvailable() const override;
		virtual bool canRead(bytesize_t numBytes) override;
		virtual bool canWrite(bytesize_t numBytes, bool autoExpand = false) override;
		virtual void clear() override;

		/**
		 * @brief
		 *  Expands the internal buffer for direct writing to the stream, returning a pointer
		 *  to the caller to use in the actual writing of the data. This can
		 *  be used to avoid temporary buffers.
		 */
		void* expandForWrite(bytesize_t byteLength);

	private:
		StdExt::Buffer mBuffer;
		seek_t mBytesWritten;
		seek_t mSeekPosition;
	};
}

#endif // !_STD_EXT_SERIALIZE_BUFFERED_STREAM_H_
