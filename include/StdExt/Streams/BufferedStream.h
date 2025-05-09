#ifndef _STD_EXT_SERIALIZE_BUFFERED_STREAM_H_
#define _STD_EXT_SERIALIZE_BUFFERED_STREAM_H_

#include "ByteStream.h"

#include "../Buffer.h"

namespace StdExt::Streams
{
	/**
	 * @brief
	 *  Stream with a dynamically allocated buffer that will grow
	 *  to fit the max size of the contents that are written to it.
	 */
	class STD_EXT_EXPORT BufferedStream : public ByteStream
	{
	public:
		BufferedStream();
		virtual ~BufferedStream();

		void* dataPtr(size_t seekPos) const override;
		void readRaw(void* destination, size_t byteLength) override;
		void writeRaw(const void* data, size_t byteLength) override;
		void seek(size_t position) override;
		size_t getSeekPosition() const override;
		size_t bytesAvailable() const override;
		bool canRead(size_t numBytes) override;
		bool canWrite(size_t numBytes, bool autoExpand = false) override;
		void clear() override;

		/**
		 * @brief
		 *  Expands the internal buffer for direct writing to the stream, returning a pointer
		 *  to the caller to use in the actual writing of the data. This can
		 *  be used to avoid temporary buffers.
		 */
		void* expandForWrite(size_t byteLength);

	private:
		StdExt::Buffer mBuffer;
		size_t mBytesWritten;
		size_t mSeekPosition;
	};
}

#endif // !_STD_EXT_SERIALIZE_BUFFERED_STREAM_H_
