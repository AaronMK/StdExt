#include <StdExt/String.h>

#include <StdExt/Collections/Vector.h>

namespace StdExt::Serialize::Binary
{
	template<Character char_t>
	void writeString(ByteStream* stream, const StringBase<char_t>& val)
	{
		uint32_t length = StdExt::Number::template convert<uint32_t>(val.size());
		write<uint32_t>(stream, length);

		if constexpr (std::endian::native == std::endian::big)
		{
			Collections::Vector<char_t, 64> buffer;
			buffer.resize(val.size);

			for (size_t i = 0; i < length; ++i)
				buffer[i] = swap_endianness(val[i]);

			stream->writeRaw(buffer.data(), length * sizeof(char_t));
		}
		else
		{
			stream->writeRaw(val.data(), length * sizeof(char_t));
		}
	}

	template<Character char_t>
	void readString(ByteStream* stream, StringBase<char_t>* out)
	{
		uint32_t length = read<uint32_t>(stream);

		auto readData = [&](char_t* out_chars)
		{
			stream->readRaw(out_chars, length * sizeof(char_t));
			out_chars[length] = 0;

			if constexpr (std::endian::native == std::endian::big)
			{
				for (size_t i = 0; i < length; ++i)
					out_chars[i] = swap_endianness(out_chars[i]);
			}
		};

		if (length <= StdExt::String::SmallSize)
		{
			char_t buffer[StdExt::String::SmallSize + 1];
			readData(buffer);

			*out = typename StringBase<char_t>::view_t(buffer, length);
		}
		else
		{
			Collections::SharedArray<char_t> memRef(length + 1);
			readData(memRef.data());

			*out = StringBase<char_t>(memRef);
		}
	}

	template<>
	void read<CString>(ByteStream* stream, CString* out)
	{
		readString(stream, out);
	}

	template<>
	void write<CString>(ByteStream* stream, const CString& val)
	{
		writeString(stream, val);
	}

	template<>
	STD_EXT_EXPORT void read<U8String>(ByteStream* stream, U8String* out)
	{
		readString(stream, out);
	}

	template<>
	STD_EXT_EXPORT void write<U8String>(ByteStream* stream, const U8String& val)
	{
		writeString(stream, val);
	}

	template<>
	STD_EXT_EXPORT void read<U16String>(ByteStream* stream, U16String* out)
	{
		readString(stream, out);
	}

	template<>
	STD_EXT_EXPORT void write<U16String>(ByteStream* stream, const U16String& val)
	{
		writeString(stream, val);
	}

	template<>
	STD_EXT_EXPORT void read<U32String>(ByteStream* stream, U32String* out)
	{
		readString(stream, out);
	}

	template<>
	STD_EXT_EXPORT void write<U32String>(ByteStream* stream, const U32String& val)
	{
		writeString(stream, val);
	}
}