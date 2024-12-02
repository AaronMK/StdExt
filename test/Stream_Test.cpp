#include <StdExt/Streams/SocketStream.h>

#include <StdExt/String.h>
#include <StdExt/Utility.h>

#include <StdExt/Test/Test.h>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Streams;
using namespace StdExt::Serialize::Binary;

void testStreams()
{
	constexpr const char8_t* LongString = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	constexpr const char8_t* MediumString = u8"CDEFGHIJKLMNOPQRSTUVWX";
	constexpr const char8_t* SmallString = u8"GHIJKLMNOPQRST";

	std::array<U8String, 3> strings;
	strings[0] = U8String(LongString);
	strings[1] = U8String(MediumString);
	strings[2] = U8String(SmallString);
	
	std::vector<U8String> string_vec;
	string_vec.reserve(50);

	for (size_t i = 0; i < 50; ++i)
	{
		string_vec.push_back(
			strings[ rand<size_t>(0, 2) ]
		);
	}

	auto checkStreamIO = [&](ByteStream& read_stream, ByteStream& write_stream)
		{
			size_t read_index = 0;
			size_t write_index = 0;

			while (read_index < string_vec.size())
			{
				if ( read_index >= write_index || 
				     ( write_index < string_vec.size() && rand<size_t>(0, 5) > 0 )
				)
				{
					write(&write_stream, string_vec[write_index++]);
				}
				else
				{
					U8String stream_out = read<U8String>(&read_stream);

					if (stream_out != string_vec[read_index++])
						throw std::runtime_error("Unexpected string from SocketString in test.");
				}
			}

			return true;
		};

	
	SocketStream ss;

	testForResult<bool>(
		"SocketStream inputs and outputs data as expected.",
		true, checkStreamIO(ss, ss)
	);

}