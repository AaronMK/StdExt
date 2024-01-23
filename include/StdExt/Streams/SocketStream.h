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

		SocketStream(SocketStream&&) = default;
		SocketStream(const SocketStream&) = delete;

		SocketStream& operator=(SocketStream&&) = default;
		SocketStream& operator=(const SocketStream&) = delete;
		
		SocketStream();
		virtual ~SocketStream();

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
		 *  avoid temporary buffers.  The internal marker for writing is also moved forward
		 *  by byteLength.
		 */
		void* expandForWrite(size_t byteLength);

		/**
		 * @brief
		 *  Facilitates direct writing to the buffer within the stream when using a function
		 *  that will write to an output buffer and return the number of bytes written.
		 * 
		 * @details
		 *  The internal buffer will be increased in size to accomodate the max read, but the
		 *  internal write marker will only progress by the amount actually read.  If read_func
		 *  throws an exception, the write marker will not progress and anything written before
		 *  the exception will be ignored.
		 */
		template<CallableWith<size_t, void*, size_t> func_t>
		void write(size_t max_read, const func_t& read_func)
		{
			mWriteMarker += read_func(reserve(max_read), max_read);
		}

		/**
		 * @brief
		 *  Reset the stream, discarding any data not yet read.
		 */
		void clear() override;

	private:
		Buffer mBuffer;

		size_t mReadMarker;
		size_t mWriteMarker;

		/**
		 * @brief
		 *  Discards data that has already been read by moving the data that has not been read
		 *  to the beginning of the internal buffer, and setting the read and write markers
		 *  accordingly.  This provides additional space for writes before additional allocation
		 *  is required.
		 */
		void discardAlreadyRead();

		/**
		 * @brief
		 *  Ensures that the space on the internal buffer is already available or otherwise
		 *  allocated for a write of <i>additional_bytes</i> length.  It does this by discarding
		 *  already read bytes and/or additional memory allocation.
		 */
		void* reserve(size_t additional_bytes);
	};
}

#endif // !_STD_EXT_STREAMS_SOCKET_STREAM_H_