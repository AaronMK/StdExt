#ifndef _STD_EXT_STREAMS_SOCKET_STREAM_H_
#define _STD_EXT_STREAMS_SOCKET_STREAM_H_

#include "ByteStream.h"

#include "../Buffer.h"

namespace StdExt::Streams
{
	/**
	 * @brief
	 *  A stream that is optimized for single write and single read behavior.  The internal
	 *  buffer management is optimized for writes the feed consumption, and reads that consider
	 *  read data to be consumed and safe for internal deletion.
	 */
	class STD_EXT_EXPORT SocketStream : public ByteStream
	{
	public:
		SocketStream();
		virtual ~SocketStream();

		SocketStream(SocketStream&&);
		SocketStream(const SocketStream&) = delete;

		SocketStream& operator=(SocketStream&&);
		SocketStream& operator=(const SocketStream&) = delete;

		virtual void* dataPtr(size_t seekPos) const override;

		virtual void readRaw(void* destination, size_t byteLength) override;

		virtual void writeRaw(const void* data, size_t byteLength) override;

		virtual size_t bytesAvailable() const override;

		virtual bool canRead(size_t numBytes) override;

		virtual bool canWrite(size_t numBytes, bool autoExpand = false) override;
		
		/**
		 * @brief
		 *  Expands the internal buffer for direct writing to the stream, returning a pointer
		 *  to the caller to use in the actual writing of the data. This can be used to 
		 *  avoid temporary buffers.  The internal maker for writing is also moved forward
		 *  by byteLength.
		 */
		void* expandForWrite(size_t byteLength);

		/**
		 * @brief
		 *  Reset the stream, discarding any data not yet read.
		 */
		void clear();

	private:
		Buffer mBuffer;

		size_t mReadMarker;
		size_t mWriteMarker;

		void discardAlreadyRead();
	};
}

#endif // !_STD_EXT_STREAMS_SOCKET_STREAM_H_