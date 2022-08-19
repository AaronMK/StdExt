#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Serialize/Text/Text.h>
#include <StdExt/Serialize/XML/XML.h>

#include <StdExt/Streams/BufferedStream.h>

#include <StdExt/Test/Test.h>

#include <StdExt/Compare.h>
#include <StdExt/String.h>
#include <StdExt/Utility.h>

#include <compare>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Streams;

struct TestSerializable
{
	int32_t mInt32{};
	float64_t mFloat64{};
	String mString{};

	bool operator==(const TestSerializable& rhs) const
	{
		return ( mInt32 == rhs.mInt32 && 
		         approxEqual(mFloat64, rhs.mFloat64) &&
		         0 == compare(mString, rhs.mString)
		);
	}
};

namespace StdExt::Serialize
{
	namespace Text
	{
		template<>
		void read(const StdExt::String& string, TestSerializable* out)
		{
			auto strings = string.split(u8", ");

			out->mInt32 = read<int32_t>(strings[0]);
			out->mFloat64 = read<float64_t>(strings[1]);
			out->mString = read<String>(strings[2]);
		}

		template<>
		StdExt::String write<TestSerializable>(const TestSerializable& val)
		{
			std::array<String, 3> strings;
			strings[0] = write(val.mInt32);
			strings[1] = write(val.mFloat64);
			strings[2] = write(val.mString);

			return String::join(strings, u8", ");
		}
	}
	
	namespace Binary
	{
		template<>
		void read(ByteStream* stream, TestSerializable* out)
		{
			out->mInt32 = read<int32_t>(stream);
			out->mFloat64 = read<float64_t>(stream);
			out->mString = read<String>(stream);
		}

		template<>
		void write(ByteStream* stream, const TestSerializable& out)
		{
			write(stream, out.mInt32);
			write(stream, out.mFloat64);
			write(stream, out.mString);
		}
	}

	namespace XML
	{
		template<>
		void read(const Element& element, TestSerializable* out)
		{
			out->mInt32 = element.getChild<int32_t>(u8"Int32");
			out->mFloat64 = element.getChild<float64_t>(u8"Float64");
			out->mString = element.getChild<String>(u8"String");
		}

		template<>
		void write(Element& element, const TestSerializable& val)
		{
			element.addChild(u8"Int32", val.mInt32);
			element.addChild(u8"Float64", val.mFloat64);
			element.addChild(u8"String", val.mString);
		}
	}
}

template<Arithmetic T>
void testArithmeticBinary()
{
	BufferedStream bs;
	T num = rand<T>();

	Serialize::Binary::write(&bs, num);
	bs.seek(0);

	T deserialized_num = Serialize::Binary::read<T>(&bs);

	std::string msg_str("Core Binary Serialize Test: ");
	msg_str += std::string(typeid(T).name());

	testForResult<T>(msg_str, num, deserialized_num);
}

template<Arithmetic T>
void testArithmeticString()
{
	T num = rand<T>();

	auto num_serialized = Serialize::Text::write(num);
	T deserialized_num = Serialize::Text::read<T>(num_serialized);

	std::string msg_str("Core String Serialize Test: ");
	msg_str += std::string(typeid(T).name());

	testForResult<T>(msg_str, num, deserialized_num);
}

template<Arithmetic T>
void testArithmeticXml()
{
	T num = rand<T>();

	Serialize::XML::Element element( String::literal(u8"TestElement") );
	Serialize::XML::write(element, num);

	std::string msg_str("Core XML Serialize Test: ");
	msg_str += std::string(typeid(T).name());

	testForResult<T>( msg_str, num, Serialize::XML::read<T>(element) );
}

void testSerialize()
{
	testArithmeticString<int16_t>();
	testArithmeticString<int32_t>();
	testArithmeticString<int64_t>();
	testArithmeticString<uint16_t>();
	testArithmeticString<uint32_t>();
	testArithmeticString<uint64_t>();
	testArithmeticString<float32_t>();
	testArithmeticString<float64_t>();

	testArithmeticBinary<int16_t>();
	testArithmeticBinary<int32_t>();
	testArithmeticBinary<int64_t>();
	testArithmeticBinary<uint16_t>();
	testArithmeticBinary<uint32_t>();
	testArithmeticBinary<uint64_t>();
	testArithmeticBinary<float32_t>();
	testArithmeticBinary<float64_t>();

	testArithmeticXml<int16_t>();
	testArithmeticXml<int32_t>();
	testArithmeticXml<int64_t>();
	testArithmeticXml<uint16_t>();
	testArithmeticXml<uint32_t>();
	testArithmeticXml<uint64_t>();
	testArithmeticXml<float32_t>();
	testArithmeticXml<float64_t>();

	{
		TestSerializable ts;
		ts.mFloat64 = StdExt::rand<float64_t>();
		ts.mInt32 = StdExt::rand<int32_t>();
		ts.mString = String::literal(u8"Test String");

		auto ts_serialized = Serialize::Text::write(ts);
		auto ts_deserialized = Serialize::Text::read<TestSerializable>(ts_serialized);

		testForResult<TestSerializable>(
			"String Serialization of Class Test",
			ts, ts_deserialized
		);
	}

	{
		BufferedStream bs;
		
		TestSerializable ts;
		ts.mFloat64 = StdExt::rand<float64_t>();
		ts.mInt32 = StdExt::rand<int32_t>();
		ts.mString = String::literal(u8"Test String");

		Serialize::Binary::write(&bs, ts);
		bs.seek(0);

		TestSerializable ts_deserialized;
		Serialize::Binary::read<TestSerializable>(&bs, &ts_deserialized);

		testForResult(
			"Binary Serialization of Class Test",
			ts, ts_deserialized
		);
	}

	{
		Serialize::XML::Element element(String::literal(u8"TestElement"));

		TestSerializable ts;
		ts.mFloat64 = StdExt::rand<float64_t>();
		ts.mInt32 = StdExt::rand<int32_t>();
		ts.mString = String::literal(u8"Test String");

		Serialize::XML::write(element, ts);

		TestSerializable ts_deserialized;
		Serialize::XML::read<TestSerializable>(element, &ts_deserialized);

		testForResult(
			"XML Serialization of Class Test",
			ts, ts_deserialized
		);
	}
}