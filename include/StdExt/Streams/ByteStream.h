#ifndef _STD_EXT_STREAMS_BYTE_STREAM_H_
#define _STD_EXT_STREAMS_BYTE_STREAM_H_

#include "../StdExt.h"

#include <type_traits>
#include <exception>
#include <string>
#include <limits>
#include <tuple>

namespace StdExt::Streams
{
	/**
	 * Base class for all data streams. 
	 */
	class STD_EXT_EXPORT ByteStream
	{
	public:
		/**
		 * @brief
		 *  Flags that are used to set and/or determine properties of a ByteStream.
		 */
		enum Flags : uint32_t
		{
			/**
			 * @brief
			 *  Stream is read only.
			 */
			NO_FLAGS = 0,

			/**
			 * @brief
			 *  Stream is read only.
			 */
			READ_ONLY = 1,

			/**
			 * @brief
			 *  Stream is write only.
			 */
			WRITE_ONLY = 2,

			/**
			 * @brief
			 *  Stream supports seeking.  Usually streams that do not support seeking
			 *  are sockets.
			 */
			CAN_SEEK = 4,

			/**
			 * @brief
			 *  Backing stream data is in memory, and can be accessed directly using pointers
			 *  returned by dataPtr().
			 */
			MEMORY_BACKED = 8,

			/**
			 * @brief
			 *  Indicates that the stream is not valid.
			 */
			INVALID = 16
		};

		ByteStream();
		virtual ~ByteStream();

		/**
		 * @brief
		 *  Shortcut to checking validity of the stream based on set flags.
		 */
		bool isValid() const;

		/**
		 * @brief
		 *  Gets a pointer to raw data at a seek position in the stream.  This does not
		 *  actually seek the stream, and will only work if the data backing the stream
		 *  is in memory.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception noting
		 *  that the stream does not support direct addressing of underlying data.
		 */
		virtual void* dataPtr(size_t seekPos) const;

		/**
		 * @brief
		 *  Either skips, or reads and ignores the specified number of bytes from the stream.
		 *
		 * @details
		 *  The default implementation will attempt to seek the stream to skip the bytes.
		 *  If that fails it will read the specified into a temporary buffer
		 *  and delete that buffer. Implementations can override this behavior for better
		 *  performance.
		 */
		virtual void skip(size_t byteLength);

		/**
		 * @brief
		 *  Reads a block of raw data from the current seek position of the file.
		 *  The seek position is moved by the number of bytes read.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void readRaw(void* destination, size_t byteLength);

		/**
		 * @brief
		 *  Writes raw data to the file. The seek position is moved
		 *  by the number of bytes written.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void writeRaw(const void* data, size_t byteLength);

		/**
		 * @brief
		 *  Seeks to the position in terms of number of bytes from the beginning.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void seek(size_t position);

		/*
		 * @brief
		 *  Gets the current seek position on which read and write operations will take place,
		 *  if applicable.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual size_t getSeekPosition() const;

		/**
		 * @brief
		 *  Gets the number of bytes available for reading from the current seek position.
		 */
		virtual size_t bytesAvailable() const = 0;

		/**
		 * @brief
		 *  Determines if there is the specified amount of data left to read.
		 */
		virtual bool canRead(size_t numBytes) = 0;

		/**
		 * @brief
		 *  Determines if the stream is capable of handling a write of the requested
		 *  size at the current seek position.  If autoExpand is true, the ByteStream
		 *  will attempt to expand to make it capable of handling the requested write
		 *  and return true if successful.
		 */
		virtual bool canWrite(size_t numBytes, bool autoExpand = false) = 0;
		
		/**
		 * @brief
		 *  For writable streams, clears all contents and resets the seek position to 0.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void clear();

		/*
		 * @brief
		 *  Gets the flags for common properties of ByteStreams.
		 */
		uint32_t getFlags() const;

	protected:
		void setFlags(uint32_t mask);

	private:
		uint32_t mFlags;
	};
}

#endif // _STD_EXT_STREAMS_BYTE_STREAM_H_