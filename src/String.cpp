#include <StdExt/String.h>
#include <StdExt/Number.h>

#include <StdExt/Collections/Vector.h>

#include <locale>
#include <vector>
#include <cuchar>

namespace StdExt
{
	using cvt_result = std::codecvt_base::result;

	template<Character InternT, Character ExternT>
		requires (!std::same_as<InternT, ExternT>)
	class CodeConvert : protected std::codecvt<InternT, ExternT, std::mbstate_t>
	{
		using base_t = std::codecvt<InternT, ExternT, std::mbstate_t>;
	public:
		CodeConvert() = default;
		virtual ~CodeConvert() {};

		cvt_result doConvert(
			std::mbstate_t& state,
			const InternT* from,
			const InternT* from_end,
			const InternT*& from_next,
			ExternT* to,
			ExternT* to_end,
			ExternT*& to_next
		) const
		{
			return base_t::out(
				state,
				from, from_end, from_next,
				to, to_end, to_next
			);
		}

		cvt_result doConvert(
			std::mbstate_t& state,
			const ExternT* from,
			const ExternT* from_end,
			const ExternT*& from_next,
			InternT* to,
			InternT* to_end,
			InternT*& to_next
		) const
		{
			return base_t::in(
				state,
				from, from_end, from_next,
				to, to_end, to_next
			);
		}
	};

	template<>
	class CodeConvert<char8_t, char16_t> : public CodeConvert<char16_t, char8_t>
	{
	};

	template<>
	class CodeConvert<char8_t, char32_t> : public CodeConvert<char32_t, char8_t>
	{
	};

	template<>
	class CodeConvert<char, wchar_t> : public CodeConvert<wchar_t, char>
	{
	};

	template<Character InternT, Character ExternT>
	static StringBase<InternT> convert(const StringBase<ExternT>& str)
	{
		constexpr auto ok = std::codecvt_base::ok;

		if (str.size() == 0)
			return StringBase<InternT>();

		CodeConvert<InternT, ExternT> converter;

		std::array<InternT, 128> buffer;
		std::vector< StringBase<InternT> > cvt_parts;

		std::mbstate_t state{};
		std::codecvt_base::result result = std::codecvt_base::noconv;

		const ExternT* from = str.data();
		const ExternT* from_end = from + str.size();
		const ExternT* from_next = from;

		while (ok != result)
		{
			InternT* to = &buffer[0];
			InternT* to_end = to + buffer.size() - 1;
			InternT* to_next = to;

			result = converter.doConvert(
				state,
				from, from_end, from_next,
				to, to_end, to_next
			);

			if (std::codecvt_base::error == result)
				throw std::runtime_error("Encountered a character that could not be converted.");

			if (std::codecvt_base::noconv == result)
				throw std::runtime_error("This facet is non-converting, no output written.");

			cvt_parts.emplace_back(to, std::distance(to, to_next));
			from = from_next;
		}

		return StringBase<InternT>::join(
			std::span< const StringBase<InternT> >(
				cvt_parts.data(), cvt_parts.size()
				)
		);
	}

	template<>
	StringBase<char> convertString<char>(const StringBase<char>& str)
	{
		return str;
	}

	template<>
	StringBase<char> convertString<char>(const StringBase<char8_t>& str)
	{
		return convertString<char>(
			convertString<char32_t>(str)
			);
	}

	template<>
	StringBase<char> convertString<char>(const StringBase<char16_t>& str)
	{
		return convertString<char>(
			convertString<char32_t>(str)
			);
	}

	template<>
	StringBase<char> convertString<char>(const StringBase<char32_t>& str)
	{
		if (str.size() == 0)
			return StringBase<char>();

		constexpr auto failure = (size_t)-1;

		auto view = str.view();

		std::mbstate_t state{};
		std::vector<char> out_buffer(std::max(MB_CUR_MAX, MB_LEN_MAX));
		std::vector<char> out_chars{};

		for (auto curr_char : view)
		{
			size_t result = c32rtomb(
				out_buffer.data(), curr_char, &state
			);

			if (failure == result)
				throw invalid_operation("String conversion failed.");

			if (result > 0)
			{
				std::span<char> add_span(out_buffer.data(), result);
				out_chars.insert(
					out_chars.end(),
					add_span.begin(), add_span.end()
				);
			}
		}

		return StringBase<char>(out_chars.data(), out_chars.size());
	}

	template<>
	StringBase<char> convertString<char>(const StringBase<wchar_t>& str)
	{
		return convert<char>(str);
	}



	template<>
	StringBase<char8_t> convertString<char8_t>(const StringBase<char>& str)
	{
		return convert<char8_t>(
			convertString<char32_t>(str)
			);
	}

	template<>
	StringBase<char8_t> convertString<char8_t>(const StringBase<char8_t>& str)
	{
		return str;
	}

	template<>
	StringBase<char8_t> convertString<char8_t>(const StringBase<char16_t>& str)
	{
		return convert<char8_t>(str);
	}

	template<>
	StringBase<char8_t> convertString<char8_t>(const StringBase<char32_t>& str)
	{
		return convert<char8_t>(str);
	}

	template<>
	StringBase<char8_t> convertString<char8_t>(const StringBase<wchar_t>& str)
	{
		return convertString<char8_t>(
			convertString<char>(str)
			);
	}



	template<>
	StringBase<char16_t> convertString<char16_t>(const StringBase<char>& str)
	{
		return convert<char16_t>(
			convertString<char32_t>(str)
			);
	}

	template<>
	StringBase<char16_t> convertString<char16_t>(const StringBase<char8_t>& str)
	{
		return convert<char16_t>(str);
	}

	template<>
	StringBase<char16_t> convertString<char16_t>(const StringBase<char16_t>& str)
	{
		return str;
	}

	template<>
	StringBase<char16_t> convertString<char16_t>(const StringBase<char32_t>& str)
	{
		return convert<char16_t>(str);
	}

	template<>
	StringBase<char16_t> convertString<char16_t>(const StringBase<wchar_t>& str)
	{
		return convertString<char16_t>(
			convertString<char>(str)
			);
	}



	template<>
	StringBase<char32_t> convertString<char32_t>(const StringBase<char>& str)
	{
		if (str.size() == 0)
			return StringBase<char32_t>{};

		constexpr auto char_written = (std::size_t)-3;
		constexpr auto char_incomplete = (std::size_t)-2;
		constexpr auto encoding_error = (std::size_t)-1;
		constexpr auto char_null = 0;

		const char* start = str.data();
		const char* last = start + (str.size() - 1);

		std::vector<char32_t> out_chars;
		std::mbstate_t state{};

		auto stringOutChars = [&]()
		{
			return StringBase<char32_t>(
				out_chars.data(), out_chars.size()
				);
		};

		while (start <= last)
		{
			char32_t char_out = 0;
			size_t result = mbrtoc32(
				&char_out, start,
				std::distance(start, last) + 1,
				&state
			);

			if (encoding_error == result)
				throw invalid_operation("Encoding Error");

			if (char_null == result)
				return stringOutChars();

			if (char_incomplete == result)
				continue;

			if (char_written == result)
			{
				out_chars.emplace_back(char_out);
				continue;
			}

			out_chars.emplace_back(char_out);
			start += result;
		}

		return stringOutChars();
	}

	template<>
	StringBase<char32_t> convertString<char32_t>(const StringBase<char8_t>& str)
	{
		return convert<char32_t>(str);
	}

	template<>
	StringBase<char32_t> convertString<char32_t>(const StringBase<char16_t>& str)
	{
		return convert<char32_t>(str);
	}

	template<>
	StringBase<char32_t> convertString<char32_t>(const StringBase<char32_t>& str)
	{
		return str;
	}

	template<>
	StringBase<char32_t> convertString<char32_t>(const StringBase<wchar_t>& str)
	{
		return convertString<char32_t>(
			convertString<char>(str)
			);
	}



	template<>
	StringBase<wchar_t> convertString<wchar_t>(const StringBase<char>& str)
	{
		return convert<wchar_t>(str);
	}

	template<>
	StringBase<wchar_t> convertString<wchar_t>(const StringBase<char8_t>& str)
	{
		return convertString<wchar_t>(
			convertString<char32_t>(str)
			);
	}

	template<>
	StringBase<wchar_t> convertString<wchar_t>(const StringBase<char16_t>& str)
	{
		return convertString<wchar_t>(
			convertString<char32_t>(str)
			);
	}

	template<>
	StringBase<wchar_t> convertString<wchar_t>(const StringBase<char32_t>& str)
	{
		return convert<wchar_t>(
			convertString<char>(str)
			);
	}

	template<>
	StringBase<wchar_t> convertString<wchar_t>(const StringBase<wchar_t>& str)
	{
		return str;
	}
}

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
	void read<U8String>(ByteStream* stream, U8String* out)
	{
		readString(stream, out);
	}

	template<>
	void write<U8String>(ByteStream* stream, const U8String& val)
	{
		writeString(stream, val);
	}

	template<>
	void read<U16String>(ByteStream* stream, U16String* out)
	{
		readString(stream, out);
	}

	template<>
	void write<U16String>(ByteStream* stream, const U16String& val)
	{
		writeString(stream, val);
	}

	template<>
	void read<U32String>(ByteStream* stream, U32String* out)
	{
		readString(stream, out);
	}

	template<>
	void write<U32String>(ByteStream* stream, const U32String& val)
	{
		writeString(stream, val);
	}
}