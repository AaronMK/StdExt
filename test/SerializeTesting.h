#ifndef SERIALIZE_TESTING_H
#define SERIALIZE_TESTING_H

#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Serialize/Text/Text.h>
#include <StdExt/Serialize/XML/XML.h>

#include <StdExt/Test/Test.h>
#include <StdExt/Streams/BufferedStream.h>

template<StdExt::Defaultable T>
void testTextSerialize(const T& val)
{
	std::string message("Text Serialize Test: ");
	message += typeid(T).name();

	StdExt::Test::testForResult<T>(
		message,
		val, 
		StdExt::Serialize::Text::read<T>(
			StdExt::Serialize::Text::write<T>(val)
		)
	);
}

template<StdExt::Defaultable T>
void testBinarySerialize(const T& val)
{
	std::string message("Binary Serialize Test: ");
	message += typeid(T).name();

	StdExt::Streams::BufferedStream bs;

	StdExt::Serialize::Binary::write<T>(&bs, val);
	bs.seek(0);
	
	StdExt::Test::testForResult<T>(
		message, val, StdExt::Serialize::Binary::read<T>(&bs)
	);
}

template<StdExt::Defaultable T>
void testXmlSerialize(const T& val)
{
	std::string message("XML Serialize Test: ");
	message += typeid(T).name();

	StdExt::Serialize::XML::Element element( StdExt::String::literal(u8"TestElement") );

	StdExt::Serialize::XML::write<T>(element, val);

	StdExt::Test::testForResult<T>(
		message, val, StdExt::Serialize::XML::read<T>(element)
	);
}

#endif // SERIALIZE_TESTING_H