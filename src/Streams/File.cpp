#include <StdExt/Streams/File.h>
#include <StdExt/Number.h>

#include <stdexcept>

#ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
#endif

#include <climits>
#include <cstdio>
#include <stdexcept>

using namespace std;

namespace StdExt::Streams
{
	File::File()
		: mFile(nullptr)
	{
	}

	File::File(const char* path, bool readonly)
		: mFile(nullptr)
	{
		open(path, readonly);
	}

	File::File(File &&other)
	{
		mFile = other.mFile;
		other.mFile = nullptr;
	}

	File::~File()
	{
		close();
	}

	void File::readRaw(void* destination, size_t byteLength)
	{
		if (0 == byteLength)
			return;

		if ((getFlags() & WRITE_ONLY) != 0)
			throw invalid_operation("Attempting to read a write-only stream.");

		if (nullptr == mFile)
			throw invalid_operation("Attempting to read on an unopened file.");

		size_t byteCount = byteLength;
		size_t elementsRead = fread(destination, byteCount, 1, mFile);

		if (elementsRead != byteCount)
		{
			if (0 != feof(mFile))
				throw out_of_range("Attempted to read passed the end of the file.");
			else
				throw std::runtime_error("Unknown file error.");
		}
	}

	void File::writeRaw(const void* data, size_t byteLength)
	{
		if (nullptr == mFile)
			throw invalid_operation("Attempting to write on an unopened file.");

		if ((getFlags() & READ_ONLY) != 0)
			throw invalid_operation("Attempting to write on a read only stream.");

		size_t byteCount = byteLength;
		size_t elementsWritten = fwrite(data, byteCount, 1, mFile);

		if (elementsWritten != 1)
		{
			if (0 != feof(mFile))
				throw out_of_range("Attempted to write passed the end of the file.");
			else
				throw std::runtime_error("Unknown file error.");
		}
	}

	void File::seek(size_t position)
	{
		long l_position = Number::convert<long>(position);

		// Check for overflow
		if (0 != fseek(mFile, l_position, SEEK_SET))
			throw out_of_range("Attempted to seek outside the bounds of the file.");
	}

	size_t File::getSeekPosition() const
	{
		if (nullptr != mFile)
			return ftell(mFile);

		return 0;
	}

	size_t File::bytesAvailable() const
	{
		File* nonConstFile = const_cast<File*>(this);

		size_t currentSeek = nonConstFile->getSeekPosition();
		size_t maxSeek = 0;

		size_t ret;

		fseek(mFile, 0, SEEK_END);
		maxSeek = nonConstFile->getSeekPosition();

		ret = maxSeek - currentSeek;

		nonConstFile->seek(currentSeek);
		return ret;
	}

	bool File::canRead(size_t numBytes)
	{
		if (nullptr == mFile)
			return false;

		long int currentSeek = ftell(mFile);

		if (currentSeek >= static_cast<long int>(std::numeric_limits<size_t>::max()))
			return false;

		size_t maxSeek = 0;

		if (0 == fseek(mFile, 0, SEEK_END))
		{
			maxSeek = ftell(mFile);
			bool ret = (numBytes <= maxSeek - currentSeek);

			seek(currentSeek);
			return ret;
		}

		return false;
	}

	bool File::canWrite(size_t numBytes, bool autoExpand)
	{
		return ( 0 == ( getFlags() & READ_ONLY) );
	}


	void File::clear()
	{
		if (0 != (getFlags() & READ_ONLY))
			throw invalid_operation("Attampting to clear a read-only file stream.");

		if ( nullptr != mFile )
		{
			FILE* reopenedFile;

			if (0 == freopen_s(&reopenedFile, nullptr, "w+", mFile))
				mFile = reopenedFile;
		}
	}

	bool File::open(StdExt::String path, bool readonly)
	{
		StdExt::String ntPath = path.getNullTerminated();

		if (nullptr != mFile)
			return false;

		if (readonly)
		{
			setFlags(READ_ONLY | CAN_SEEK);
			fopen_s(&mFile, ntPath.data(), "rb");
		}
		else
		{
			setFlags(CAN_SEEK);

			if ( exists(ntPath.data()) )
				fopen_s(&mFile, ntPath.data(), "rb+");
			else
				fopen_s(&mFile, ntPath.data(), "ab+");
		}

		if (mFile == nullptr)
			perror("fopen");

		return (nullptr != mFile);
	}

	void File::close()
	{
		
		fclose(mFile);
		mFile = nullptr;
	}

	bool File::isOpen()
	{
		return (nullptr != mFile);
	}

	FILE* File::rawHandle()
	{
		return mFile;
	}

	File& File::operator=(File &&other)
	{
		if (mFile != other.mFile)
		{
			if (mFile)
				fclose(mFile);

			mFile = other.mFile;
			other.mFile = nullptr;
		}

		return *this;
	}

	bool File::exists(const char* path)
	{
		FILE* testFile;
		fopen_s(&testFile, path, "rb");

		if (nullptr != testFile)
		{
			fclose(testFile);
			return true;
		}

		return false;
	}
}