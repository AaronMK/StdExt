#ifndef _STD_EXT_SERIALIZE_BYTE_STREAM_H_
#define _STD_EXT_SERIALIZE_BYTE_STREAM_H_

#include "Binary.h"

#include <type_traits>
#include <exception>
#include <string>
#include <limits>
#include <tuple>

namespace StdExt::Serialize::Binary
{
	/**
	 * Base class for all data streams. 
	 */
	class STD_EXT_EXPORT ByteStream
	{
	private:

		template<typename... tuple_types>
		struct TupleReader
		{
			typedef std::tuple<tuple_types...> tuple_t;

			ByteStream* mInStream;
			tuple_t* mOutTuple;

			template<size_t index>
			void readIndex()
			{
				std::get<index>(*mOutTuple) =
					mInStream->read<std::tuple_element<index, tuple_t>::type>();
			}

			template<size_t index>
			void read()
			{
				readIndex<index>();
				readIndex<index + 1>();
			}

			template<>
			void read<sizeof...(tuple_types) - 1>()
			{
				readIndex<sizeof...(tuple_types) - 1>();
			}

			TupleReader(tuple_t* outTuple, ByteStream* inStream)
			{
				mInStream = inStream;
				mOutTuple = outTuple;

				read<0>();
			}
		};

		template<typename... tuple_types>
		struct TupleWriter
		{
			typedef std::tuple<tuple_types...> tuple_t;

			ByteStream* mOutStream;
			const tuple_t* mInTuple;

			template<size_t index>
			void writeIndex()
			{
				mOutStream->write(std::get<index>(*mInTuple));
			}

			template<size_t index>
			void write()
			{
				writeIndex<index>();
				writeIndex<index + 1>();
			}

			template<>
			void write<sizeof...(tuple_types) - 1>()
			{
				writeIndex<sizeof...(tuple_types) - 1>();
			}

			TupleWriter(const tuple_t* inTuple, ByteStream* outStream)
			{
				mInTuple = inTuple;
				mOutStream = outStream;

				write<0>();
			}
		};

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
		virtual void* dataPtr(seek_t seekPos) const;

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
		virtual void skip(bytesize_t byteLength);

		/**
		 * @brief
		 *  Reads a block of raw data from the current seek position of the file.
		 *  The seek position is moved by the number of bytes read.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void readRaw(void* destination, bytesize_t byteLength);

		/**
		 * @brief
		 *  Writes raw data to the file. The seek position is moved
		 *  by the number of bytes written.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void writeRaw(const void* data, bytesize_t byteLength);

		/**
		 * @brief
		 *  Seeks to the position in terms of number of bytes from the beginning.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual void seek(seek_t position);

		/*
		 * @brief
		 *  Gets the current seek position on which read and write operations will take place,
		 *  if applicable.
		 *
		 * @details
		 *  The default implementation will throw an InvalidOperation exception.
		 */
		virtual seek_t getSeekPosition() const;

		/**
		 * @brief
		 *  Gets the number of bytes available for reading from the current seek position.
		 */
		virtual bytesize_t bytesAvailable() const = 0;

		/**
		 * @brief
		 *  Determines if there is the specified amount of data left to read.
		 */
		virtual bool canRead(bytesize_t numBytes) = 0;

		/**
		 * @brief
		 *  Determines if the stream is capable of handling a write of the requested
		 *  size at the current seek position.  If autoExpand is true, the ByteStream
		 *  will attempt to expand to make it capable of handling the requested write
		 *  and return true if successful.
		 */
		virtual bool canWrite(bytesize_t numBytes, bool autoExpand = false) = 0;
		
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

		template<typename T>
		T read()
		{
			return Serialize::Binary::read<T>(this);
		}

		template<typename T>
		void write(const T& value)
		{
			Serialize::Binary::write<T>(this, value);
		}

		template<typename... tuple_types>
		std::tuple<tuple_types...> readTuple()
		{
			std::tuple<tuple_types...> outTuple;
			TupleReader<tuple_types...> reader(&outTuple, this);

			return outTuple;
		}

		template<typename... tuple_types>
		void writeTuple(const std::tuple<tuple_types...>& outTuple)
		{
			TupleWriter<tuple_types...> writer(&outTuple, this);
		}

	protected:
		void setFlags(uint32_t mask);

	private:
		uint32_t mFlags;
	};
}

#endif // _STD_EXT_SERIALIZE_BYTE_STREAM_H_