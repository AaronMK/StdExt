#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Serialize/Text/Text.h>
#include <StdExt/Serialize/XML/XML.h>

#include <StdExt/Streams/BufferedStream.h>

#include <StdExt/Test/Test.h>
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
		         0 == mString.compare(rhs.mString)
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
			auto strings = string.split(", ");

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

			return String::join(strings, ", ");
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
			out->mInt32 = element.getChild<int32_t>("Int32");
			out->mFloat64 = element.getChild<float64_t>("Float64");
			out->mString = element.getChild<String>("String");
		}

		template<>
		void write(Element& element, const TestSerializable& val)
		{
			element.addChild("Int32", val.mInt32);
			element.addChild("Float64", val.mFloat64);
			element.addChild("String", val.mString);
		}
	}
}

void testSerialize()
{
	{
		TestSerializable ts;
		ts.mFloat64 = StdExt::rand<float64_t>();
		ts.mInt32 = StdExt::rand<int32_t>();
		ts.mString = StringLiteral("Test String");

		auto ts_serialized = Serialize::Text::write(ts);

		auto ts_deserialized = Serialize::Text::read<TestSerializable>(ts_serialized);

		testForResult<TestSerializable>(
			"String Serialization Test",
			ts, ts_deserialized
		);
	}

	{
		BufferedStream bs;
		
		TestSerializable ts;
		ts.mFloat64 = StdExt::rand<float64_t>();
		ts.mInt32 = StdExt::rand<int32_t>();
		ts.mString = StringLiteral("Test String");

		Serialize::Binary::write(&bs, ts);
		bs.seek(0);

		TestSerializable ts_deserialized;
		Serialize::Binary::read<TestSerializable>(&bs, &ts_deserialized);

		testForResult(
			"String Serialization Test",
			ts, ts_deserialized
		);
	}

	{
		Serialize::XML::Element element(StringLiteral("TestElement"));

		TestSerializable ts;
		ts.mFloat64 = StdExt::rand<float64_t>();
		ts.mInt32 = StdExt::rand<int32_t>();
		ts.mString = StringLiteral("Test String");

		Serialize::XML::write(element, ts);

		TestSerializable ts_deserialized;
		Serialize::XML::read<TestSerializable>(element, &ts_deserialized);

		testForResult(
			"String Serialization Test",
			ts, ts_deserialized
		);
	}
}