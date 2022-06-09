#include <StdExt/Serialize/Serialize.h>
#include <StdExt/Serialize/Exceptions.h>
#include <StdExt/Serialize/Binary/Binary.h>

#include <StdExt/Buffer.h>
#include <StdExt/Memory.h>

using namespace StdExt;

namespace StdExt::Serialize::Binary
{
	template<>
	void read<bool>(ByteStream* stream, bool *out)
	{
		uint8_t numVal;
		stream->readRaw(&numVal, sizeof(numVal));

		if (0 != numVal && 1 != numVal)
			throw FormatException("Boolean should have serialized value of either 0 or 1.");
	}

	template<>
	void write<bool>(ByteStream* stream, const bool &val)
	{
		uint8_t numVal = val ? 1 : 0;
		stream->writeRaw(&numVal, sizeof(uint8_t));
	}

	template<>
	void read<uint8_t>(ByteStream* stream, uint8_t *out)
	{
		stream->readRaw(out, sizeof(uint8_t));
	}

	template<>
	void write<uint8_t>(ByteStream* stream, const uint8_t &val)
	{
		stream->writeRaw(&val, sizeof(uint8_t));
	}

	template<>
	void read<uint16_t>(ByteStream* stream, uint16_t *out)
	{
		stream->readRaw(out, sizeof(uint16_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<uint16_t>(ByteStream* stream, const uint16_t &val)
	{
		uint16_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(uint16_t));
	}

	template<>
	void read<uint32_t>(ByteStream* stream, uint32_t *out)
	{
		stream->readRaw(out, sizeof(uint32_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<uint32_t>(ByteStream* stream, const uint32_t &val)
	{
		uint32_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(uint32_t));
	}

	template<>
	void read<uint64_t>(ByteStream* stream, uint64_t *out)
	{
		stream->readRaw(out, sizeof(uint64_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<uint64_t>(ByteStream* stream, const uint64_t &val)
	{
		uint64_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(uint64_t));
	}

	template<>
	void read<int8_t>(ByteStream* stream, int8_t *out)
	{
		stream->readRaw(out, sizeof(int8_t));
	}

	template<>
	void write<int8_t>(ByteStream* stream, const int8_t &val)
	{
		stream->writeRaw(&val, sizeof(int8_t));
	}

	template<>
	void read<int16_t>(ByteStream* stream, int16_t *out)
	{
		stream->readRaw(out, sizeof(int16_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<int16_t>(ByteStream* stream, const int16_t &val)
	{
		int16_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(int16_t));
	}

	template<>
	void read<int32_t>(ByteStream* stream, int32_t *out)
	{
		stream->readRaw(out, sizeof(int32_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<int32_t>(ByteStream* stream, const int32_t &val)
	{
		int32_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(int32_t));
	}

	template<>
	void read<int64_t>(ByteStream* stream, int64_t *out)
	{
		stream->readRaw(out, sizeof(int64_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<int64_t>(ByteStream* stream, const int64_t &val)
	{
		int64_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(int64_t));
	}

	template<>
	void read<float32_t>(ByteStream* stream, float32_t *out)
	{
		stream->readRaw(out, sizeof(float32_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<float32_t>(ByteStream* stream, const float32_t &val)
	{
		float32_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(float32_t));
	}

	template<>
	void read<float64_t>(ByteStream* stream, float64_t *out)
	{
		stream->readRaw(out, sizeof(float64_t));
		*out = StdExt::from_little_endian(*out);
	}

	template<>
	void write<float64_t>(ByteStream* stream, const float64_t &val)
	{
		float64_t le = StdExt::to_little_endian(val);
		stream->writeRaw(&le, sizeof(float64_t));
	}

	template<>
	void read<std::string>(ByteStream* stream, std::string *out)
	{
		uint32_t length  = read<uint32_t>(stream);
		std::string str;

		str.resize(length);
		stream->readRaw((void*)str.data(), length);
		*out = std::move(str);
	}

	template<>
	void write<std::string>(ByteStream* stream, const std::string &val)
	{
		uint32_t length = static_cast<uint32_t>(val.length());

		write(stream, length);
		stream->writeRaw(val.data(), length);
	}
}