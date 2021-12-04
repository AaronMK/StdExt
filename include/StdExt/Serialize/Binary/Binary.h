#ifndef _STD_EXT_SERIALIZE_BINARY_H_
#define _STD_EXT_SERIALIZE_BINARY_H_

#include "../Serialize.h"
#include "../../Type.h"

#include <string>
#include <tuple>

namespace StdExt::Streams
{
	class ByteStream;
}

/**
 * Adding Support for New Datatypes
 * ------------------
 *
 * To add serialization support for new types, add specializations
 * read<T>() and write<T>() into the Serialize::Binary namespace.
 * These functions can simply utilize Bytestream::readRaw()
 * and Bytestream::writeRaw(), but they can also chain other read and write
 * specializations.  Take the following types for example:
 *
 * @code
 * struct xyCoord
 * {
 * 	int16_t x;
 * 	int16_t y;
 * };
 * 
 * struct WindowPos
 * {
 * 	xyCoord topLeft;
 * 	int16_t witdth;
 * 	int16_t height; 
 * };
 * @endcode 
 *
 * The following code would add serialization support for these types:
 *
 * @code
 * namespace StdExt::Serialize
 *	{
 *		template<>
 *		bool read<xyCoord>(ByteStream* stream, xyCoord *out)
 *		{
 *			xyCoord tmpOut;
 *			seek_t backSeek = stream->getSeekPosition();
 * 
 *			if (read<int16_t>(stream, &tmpOut.x) && read<int16_t>(stream, &tmpOut.y))
 *			{
 *				if (out)
 *					*out = tmpOut;
 *
 *				return true;
 *			}
 *
 *			stream->seek(backSeek);
 *			return false;
 *		}
 *
 *		template<>
 *		bool write<xyCoord>(ByteStream* stream, const xyCoord &val)
 *		{
 *			seek_t backSeek = stream->getSeekPosition();
 *
 *			if (write<int16_t>(stream, val.x) && write<int16_t>(stream, val.y))
 *				return true;
 *
 *			stream->seek(backSeek);
 *			return false;
 *		}
 *
 *		template<>
 *		bool read<WindowPos>(ByteStream* stream, WindowPos *out)
 *		{
 *			WindowPos tmpOut;
 *			seek_t backSeek = stream->getSeekPosition();
 *
 *			if ( read<xyCoord>(stream, &tmpOut.topLeft) &&
 *			     read<int16_t>(stream, &tmpOut.witdth) && 
 *			     read<int16_t>(stream, &tmpOut.height))
 *			{
 *				if (out)
 *					*out = tmpOut;
 *
 *				return true;
 *			}
 *
 *			stream->seek(backSeek);
 *			return false;
 *		}
 *
 *		template<>
 *		bool write<WindowPos>(ByteStream* stream, const WindowPos &val)
 *		{
 *			seek_t backSeek = stream->getSeekPosition();
 *
 *			if ( write<xyCoord>(stream, val.topLeft) &&
 *			     write<int16_t>(stream, val.witdth) &&
 *			     write<int16_t>(stream, val.height))
 *			{
 *				return true;
 *			}
 *
 *			stream->seek(backSeek);
 *			return false;
 *		}
 *	}
 * @endcode
 */
namespace StdExt::Serialize::Binary
{
	using ByteStream = StdExt::Streams::ByteStream;

	using float32_t = StdExt::float32_t;
	using float64_t = StdExt::float64_t;

	// Types to support bytestream metrics
	using seek_t = uint32_t;
	using bytesize_t = uint32_t;

	/**
	 * @brief
	 *  Reads data from the passed stream and de-serializes that data into a T value
	 *  that is stored in out.  If successful true is returned and stream remains
	 *  seeked passed the data read.  If unsuccessful, false is returned, and if the stream
	 *  has seeking support, is returned to its original seek position.
	 *
	 *  If out is a nullptr, the function should read as it normally would including seeking
	 *  of the stream if the read would be successful, just not storing the value as it
	 *  normally would.
	 *
	 *  If the read is not successful, the object to which out points should remain unchanged,
	 *  but in the interest of simplifying code, this is not required.
	 *  
	 *  This function must be specialized for type support.  Otherwise, the default
	 *  implementation will be compiled and generate an error. 
	 */
	template<typename T>
	void read(ByteStream* stream, T *out)
	{
		if constexpr (std::is_enum_v<T>)
			*out = readEnum<T>(stream);
		else
			static_assert(false, "StdExt::Serialize::read<T> needs to be specialized for the type T template parameter.");
	}

	/**
	 * @brief
	 *  Serializes the passed T value and writes that data to the passed stream. 
	 *  If successful true is returned and stream remains seeked passed the data
	 *  read.  If unsuccessful, false is returned. If the stream
	 *  has seeking support, is returned to its original seek position.
	 *
	 *  For non-enumeration types, this function must be specialized for type support.  
	 *  Otherwise, the default implementation will generate a static assertion failure.
	 */
	template<typename T>
	void write(ByteStream* stream, const T &val)
	{
		if constexpr (std::is_enum_v<T>)
			writeEnum<T>(stream, val);
		else
			static_assert(false, "StdExt::Serialize::write<T> needs to be specialized for the type T template parameter.");
	}

	template<>
	STD_EXT_EXPORT void read<bool>(ByteStream* stream, bool *out);

	template<>
	STD_EXT_EXPORT void write<bool>(ByteStream* stream, const bool &val);

	template<>
	STD_EXT_EXPORT void read<uint8_t>(ByteStream* stream, uint8_t *out);

	template<>
	STD_EXT_EXPORT void write<uint8_t>(ByteStream* stream, const uint8_t &val);

	template<>
	STD_EXT_EXPORT void read<uint16_t>(ByteStream* stream, uint16_t *out);

	template<>
	STD_EXT_EXPORT void write<uint16_t>(ByteStream* stream, const uint16_t &val);

	template<>
	STD_EXT_EXPORT void read<uint32_t>(ByteStream* stream, uint32_t *out);

	template<>
	STD_EXT_EXPORT void write<uint32_t>(ByteStream* stream, const uint32_t &val);

	template<>
	STD_EXT_EXPORT void read<uint64_t>(ByteStream* stream, uint64_t *out);

	template<>
	STD_EXT_EXPORT void write<uint64_t>(ByteStream* stream, const uint64_t &val);

	template<>
	STD_EXT_EXPORT void read<int8_t>(ByteStream* stream, int8_t *out);

	template<>
	STD_EXT_EXPORT void write<int8_t>(ByteStream* stream, const int8_t &val);

	template<>
	STD_EXT_EXPORT void read<int16_t>(ByteStream* stream, int16_t *out);

	template<>
	STD_EXT_EXPORT void write<int16_t>(ByteStream* stream, const int16_t &val);

	template<>
	STD_EXT_EXPORT void read<int32_t>(ByteStream* stream, int32_t *out);

	template<>
	STD_EXT_EXPORT void write<int32_t>(ByteStream* stream, const int32_t &val);

	template<>
	STD_EXT_EXPORT void read<int64_t>(ByteStream* stream, int64_t *out);

	template<>
	STD_EXT_EXPORT void write<int64_t>(ByteStream* stream, const int64_t &val);

	template<>
	STD_EXT_EXPORT void read<float32_t>(ByteStream* stream, float32_t *out);

	template<>
	STD_EXT_EXPORT void write<float32_t>(ByteStream* stream, const float32_t &val);

	template<>
	STD_EXT_EXPORT void read<float64_t>(ByteStream* stream, float64_t *out);

	template<>
	STD_EXT_EXPORT void write<float64_t>(ByteStream* stream, const float64_t &val);

	template<>
	STD_EXT_EXPORT void read<std::string>(ByteStream* stream, std::string *out);

	template<>
	STD_EXT_EXPORT void write<std::string>(ByteStream* stream, const std::string &val);

	template<>
	STD_EXT_EXPORT void write<std::string_view>(ByteStream* stream, const std::string_view &val);

	//////////////////////

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

	template<typename... tuple_types>
	std::tuple<tuple_types...> readTuple(ByteStream* stream)
	{
		std::tuple<tuple_types...> outTuple;
		TupleReader<tuple_types...> reader(&outTuple, stream);

		return outTuple;
	}

	template<typename... tuple_types>
	void writeTuple(const std::tuple<tuple_types...>& outTuple, ByteStream* stream)
	{
		TupleWriter<tuple_types...> writer(&outTuple, stream);
	}

	template<typename T>
	T readEnum(ByteStream* stream)
	{
		static_assert(std::is_enum_v<T>);
		return (T)read<std::underlying_type_t<T>>(stream);
	}

	template<typename T>
	void writeEnum(ByteStream* stream, T val)
	{
		static_assert(std::is_enum_v<T>);
		write<std::underlying_type_t<T>>(stream, (std::underlying_type_t<T>)val);
	}
	
	template<typename T>
	void read(ByteStream* stream, T *out, size_t count)
	{
		for (size_t i = 0; i < count; i++)
			read<T>(stream, &out[i]);
	}

	template<typename T>
	void write(ByteStream* stream, const T *vals, size_t count)
	{
		for (size_t i = 0; i < count; i++)
			write<T>(stream, vals[i]);
	}

	template<typename T>
	T read(ByteStream* stream)
	{
		T out;
		read(stream, &out);

		return out;
	}
}

#endif // _STD_EXT_SERIALIZE_BINARY_H_