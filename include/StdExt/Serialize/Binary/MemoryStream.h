#ifndef _STD_EXT_SERIALIZE_MEMORY_STREAM_H_
#define _STD_EXT_SERIALIZE_MEMORY_STREAM_H_

#include "ByteStream.h"

namespace StdExt::Serialize::Binary
{
	/**
	 * @brief
	 *  Maps a byte stream for reading and/or writing to a fixed size location in memory.  No
	 *  reading or writing beyond the initially specified memory block is supported.  The
	 *  stream takes no ownership of the mapped memory.
	 */
	class STD_EXT_EXPORT MemoryStream : public ByteStream
	{
	public:
		MemoryStream();

		/**
		 * @brief
		 *  Version of the constructor the points to const memory.  This will have read only access.
		 */
		MemoryStream(const void* beginning, bytesize_t size);

		/**
		 * @brief
		 *  Constructor for memory with read and write access.  READ_ONLY and WRITE_ONLY
		 *  flags can modify this behavior.
		 */
		MemoryStream(void* beginning, bytesize_t size, Flags flags = NO_FLAGS);

		virtual ~MemoryStream();

		virtual void* dataPtr(seek_t seekPos) const override;

		virtual void readRaw(void* destination, bytesize_t byteLength) override;
		virtual void writeRaw(const void* data, bytesize_t byteLength) override;
		virtual void seek(seek_t position) override;
		virtual seek_t getSeekPosition() const override;
		virtual bytesize_t bytesAvailable() const override;
		virtual bool canRead(bytesize_t numBytes) override;
		virtual bool canWrite(bytesize_t numBytes, bool autoExpand = false) override;
		virtual void clear() override;

	private:
		bytesize_t mSize;
		seek_t mSeekPosition;

		void* mData;
	};
}

#endif // _STD_EXT_SERIALIZE_MEMORY_STREAM_H_