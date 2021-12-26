#ifndef _STD_EXT_SERIALIZE_FILE_H_
#define _STD_EXT_SERIALIZE_FILE_H_

#include "ByteStream.h"

#include "../String.h"

#include <stdio.h>
#include <filesystem>

namespace StdExt::Streams
{
	/**
	 * File based ByteStream.
	 *
	 * An attempt to create files opened for writing is always made if they don't exist, and
	 * any file open for writing will also be readable.
	 */
	class STD_EXT_EXPORT FileStream : public ByteStream
	{
	public:
		FileStream(const FileStream&) = delete;
		FileStream& operator=(const FileStream&) = delete;

		FileStream();

		/**
		 * @param path
		 *    Either an absolute or relative path to the file.
		 *
		 * @param readonly
		 *    Designates whether the file will only be opened for reading. 
		 */
		FileStream(const char* path, bool readonly);

		/**
		 * Move constructor.  The entire state of other, including seek position
		 * and status, will be moved to the new object.
		 */
		FileStream(FileStream &&other);
		
		virtual ~FileStream();

		virtual void readRaw(void* destination, size_t byteLength) override;
		virtual void writeRaw(const void* data, size_t byteLength) override;
		virtual void seek(size_t position) override;
		virtual size_t getSeekPosition() const override;
		virtual size_t bytesAvailable() const override;
		virtual bool canRead(size_t numBytes) override;
		virtual bool canWrite(size_t numBytes, bool autoExpand = false) override;
		virtual void clear() override;

		bool open(StdExt::String path, bool readonly);
		void close();

		bool isOpen();

		FILE* rawHandle();

		FileStream& operator=(FileStream &&other);

		static bool exists(const char* path);

	private:
		FILE* mFile;
	};
}

#endif // _STD_EXT_SERIALIZE_FILE_H_