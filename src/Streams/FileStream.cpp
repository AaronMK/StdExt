#include <StdExt/Streams/FileStream.h>
#include <StdExt/Number.h>

#include <stdexcept>
#include <filesystem>

#ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
#endif

#include <climits>
#include <cstdio>
#include <cstring>
#include <stdexcept>

using namespace std;

#ifdef _WIN32
	static void fopen(FILE** file, const wchar_t* filename, const wchar_t* mode)
	{
		if (0 != _wfopen_s(file, filename, mode) )
			throw std::runtime_error("Failed to open file.");
	}

	static void freopen(FILE** stream, const wchar_t* mode, FILE* oldStream)
	{
		if (0 != _wfreopen_s(stream, nullptr, mode, oldStream) )
			throw std::runtime_error("Failed to reopen file.");
	}
#else
	static void fopen(FILE** file, const char* filename, const char* mode)
	{
		FILE* result = fopen64(filename, mode);

		if ( nullptr == result )
			throw std::runtime_error("Failed to open file.");

		*file = result;
	}

	static void freopen(FILE** stream, const char* mode, FILE* oldStream)
	{
		FILE* result = freopen64(nullptr, mode, oldStream);

		if ( nullptr == result )
			throw std::runtime_error("Failed to reopen file.");

		*stream = result;
	}
#endif

namespace StdExt::Streams
{
	FileStream::FileStream()
		: mFile(nullptr)
	{
	}

	FileStream::FileStream(const String& path, bool readonly)
		: mFile(nullptr)
	{
		open(path, readonly);
	}

	FileStream::FileStream(FileStream &&other)
	{
		mFile = other.mFile;
		other.mFile = nullptr;
	}

	FileStream::~FileStream()
	{
		close();
	}

	void FileStream::readRaw(void* destination, size_t byteLength)
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

	void FileStream::writeRaw(const void* data, size_t byteLength)
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

	void FileStream::seek(size_t position)
	{
		long l_position = Number::convert<long>(position);

		// Check for overflow
		if (0 != fseek(mFile, l_position, SEEK_SET))
			throw out_of_range("Attempted to seek outside the bounds of the file.");
	}

	size_t FileStream::getSeekPosition() const
	{
		if (nullptr != mFile)
			return ftell(mFile);

		return 0;
	}

	size_t FileStream::bytesAvailable() const
	{
		FileStream* nonConstFile = const_cast<FileStream*>(this);

		size_t currentSeek = nonConstFile->getSeekPosition();
		size_t maxSeek = 0;

		size_t ret;

		fseek(mFile, 0, SEEK_END);
		maxSeek = nonConstFile->getSeekPosition();

		ret = maxSeek - currentSeek;

		nonConstFile->seek(currentSeek);
		return ret;
	}

	bool FileStream::canRead(size_t numBytes)
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

	bool FileStream::canWrite(size_t numBytes, bool autoExpand)
	{
		return ( 0 == ( getFlags() & READ_ONLY) );
	}


	void FileStream::clear()
	{
		if (0 != (getFlags() & READ_ONLY))
			throw invalid_operation("Attampting to clear a read-only file stream.");

		if ( nullptr != mFile )
		{
			FILE* reopenedFile;
			freopen(&reopenedFile, L"w+", mFile);
			mFile = reopenedFile;
		}
	}

	bool FileStream::open(const String& path, bool readonly)
	{
		StdExt::WString ntPath = convertString<wchar_t>(path);

		if (nullptr != mFile)
			return false;

		if (readonly)
		{
			setFlags(READ_ONLY | CAN_SEEK);
			fopen(&mFile, ntPath.data(), L"rb");
		}
		else
		{
			setFlags(CAN_SEEK);

			if ( exists(path) )
				fopen(&mFile, ntPath.data(), L"rb+");
			else
				fopen(&mFile, ntPath.data(), L"ab+");
		}

		if (mFile == nullptr)
			perror("fopen");

		return (nullptr != mFile);
	}

	void FileStream::close()
	{
		
		fclose(mFile);
		mFile = nullptr;
	}

	bool FileStream::isOpen()
	{
		return (nullptr != mFile);
	}

	FILE* FileStream::rawHandle()
	{
		return mFile;
	}

	FileStream& FileStream::operator=(FileStream &&other)
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

	bool FileStream::exists(const String& path)
	{
		namespace fs = std::filesystem;
		return std::filesystem::exists( std::filesystem::path(path.view()) );
	}
}