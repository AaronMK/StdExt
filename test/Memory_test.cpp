#include "TestClasses.h"


#include <StdExt/Memory/BitMask.h>
#include <StdExt/Memory/Endianess.h>
#include <StdExt/Memory/SharedData.h>
#include <StdExt/Memory/SharedPtr.h>

#include <StdExt/Test/Test.h>

#include <array>
#include <string>
#include <cstdint>

using namespace std;
using namespace StdExt;
using namespace StdExt::Test;

class alignas(32) HighAlign
{
	char mData[32];
};

void testMemory()
{
	constexpr uint32_t test_uint_bits = 0x695A279C;

	static_assert(prefixMask<uint32_t>(8) == 0xFF000000);
	static_assert(prefixMask<uint32_t>(9) == 0xFF800000);
	static_assert(prefixMask<uint32_t>(32) == 0xFFFFFFFF);
	static_assert(prefixMask<uint32_t>(0) == 0);

	static_assert(postfixMask<uint32_t>(32) == 0xFFFFFFFF);
	static_assert(postfixMask<uint32_t>(0) == 0);
	static_assert(postfixMask<uint32_t>(8) == 0xFF);
	static_assert(postfixMask<uint32_t>(9) == 0x1FF);
	
	static_assert(prefixMask<uint16_t>(8) == 0xFF00);
	static_assert(prefixMask<uint16_t>(9) == 0xFF80);

	static_assert(postfixMask<uint16_t>(0) == 0);
	static_assert(postfixMask<uint16_t>(8) == 0xFF);
	static_assert(postfixMask<uint16_t>(9) == 0x1FF);

	static_assert(bitMask<uint32_t>(23, 8) == 0x00FFFF00);
	static_assert(bitMask<uint32_t>(31, 8) == 0xFFFFFF00);

	static_assert(maskBits<uint32_t>(test_uint_bits, 23, 8) == 0x5A2700);
	static_assert(maskBits<23, 8>(test_uint_bits) == 0x5A2700);

	static_assert(maskBits<uint32_t>(test_uint_bits, 27, 5) == 0x95A2780);
	static_assert(maskBits<19, 16>(test_uint_bits) == 0xA0000);

	static_assert(maskValue<uint32_t>(test_uint_bits, 27, 5) == 0x4AD13C);
	static_assert(maskValue<23, 8>(test_uint_bits) == 0x5A27);
	
#	pragma region StdExt::SharedData
	{
		const SharedData<int> const_shared_data(4);

		testForResult<int>(
			"StdExt::SharedData properly default constructs metadata.",
			0, *const_shared_data.metadata()
		);

		SharedData<int> shared_data(16);
		
		testForResult<size_t>(
			"StdExt::SharedData reports correct size.",
			16, shared_data.size()
		);
		
		testForResult<bool>(
			"StdExt::SharedData reports pointer to data after construction.",
			true, nullptr != shared_data.data()
		);

		shared_data.makeNull();
		
		testForResult<bool>(
			"StdExt::SharedData reports nullptr after makeNull() call.",
			true, nullptr == shared_data.data()
		);
	}
#	pragma endregion
	
#	pragma region StdExt::TaggedPtr
	{
		std::string str("Test string");
		uint16_t tag_val = 16;

		testForResult<std::string*>(
			"StdExt::TaggedPtr.ptr() defaults to nullptr",
			nullptr, TaggedPtr<uint16_t, std::string*>().ptr()
		);

		testForResult<uint16_t>(
			"StdExt::TaggedPtr.tag() defaults to 0",
			0, TaggedPtr<uint16_t, std::string*>().tag()
		);

#		pragma region StdExt::TaggedPtr.setPtr()
		{
			TaggedPtr<uint16_t, std::string*> tagged_ptr;
			tagged_ptr.setPtr(&str);

			testForResult<std::string*>(
				"StdExt::TaggedPtr.setPtr() correctly sets ptr()",
				&str, tagged_ptr.ptr()
			);
		}
#		pragma endregion

#		pragma region StdExt::TaggedPtr.setTag()
		{
			TaggedPtr<uint16_t, std::string*> tagged_ptr;
			tagged_ptr.setTag(tag_val);

			testForResult<uint16_t>(
				"StdExt::TaggedPtr.setTag() correctly sets tag()",
				tag_val, tagged_ptr.tag()
			);
		}
#		pragma endregion

#		pragma region StdExt::TaggedPtr.pack()
		{
			TaggedPtr<uint16_t, std::string*> tagged_ptr;
			tagged_ptr.pack(tag_val, &str);

			testForResult<uint16_t>(
				"StdExt::TaggedPtr.pack() correctly sets tag()",
				tag_val, tagged_ptr.tag()
			);

			testForResult<std::string*>(
				"StdExt::TaggedPtr.pack() correctly sets ptr()",
				&str, tagged_ptr.ptr()
			);
		}
#		pragma endregion

#		pragma region StdExt::TaggedPtr pointer dereferencing
		{
			TaggedPtr<uint16_t, std::string*> tagged_ptr;
			tagged_ptr.pack(tag_val, &str);

			testForResult<size_t>("StdExt::TaggedPtr[] correctly derferences pointer.",
				str.size(), tagged_ptr->size()
			);
		}
#		pragma endregion
	}
#	pragma endregion

#	pragma region Alignment Tests
	{
		alignas(64) char mem_Buffer[95];
		HighAlign* ha_ptr = access_as<HighAlign*>(&mem_Buffer[3]);
		size_t space = 92;
		bool ret{};

		testByCheck(
			"StdExt::align_for() success parameters.",
			[&]()
			{
				ret = align_for<HighAlign>(ha_ptr, space);
			},
			[&]() -> bool
			{
				return (
					ret &&
					ha_ptr == access_as<HighAlign*>(&mem_Buffer[32]) &&
					space == 63
				);
			}
		);

		ha_ptr = access_as<HighAlign*>(&mem_Buffer[64]);
		space = 31;

		testByCheck(
			"StdExt::align_for() failure parameters.",
			[&]()
			{
				ret = align_for<HighAlign>(ha_ptr, space);
			},
			[&]() -> bool
			{
				return (
					!ret &&
					ha_ptr == access_as<HighAlign*>(&mem_Buffer[64]) &&
					space == 31
				);
			}
		);

		testForResult<bool>(
			"StdExt::can_place_aligned() success parameters.",
			true, can_place_aligned(64, 32, 80, 16)
		);

		testForResult<bool>(
			"StdExt::can_place_aligned() failure parameters. (Destination is too small.)",
			false, can_place_aligned(64, 32, 48, 16)
		);

		testForResult<bool>(
			"StdExt::can_place_aligned() failure parameters. (No guarentee of enough space after alignemnt.)",
			false, can_place_aligned(64, 32, 80, 8)
		);

		testForResult<bool>(
			"StdExt::can_place_aligned<T>() success parameters.",
			true, can_place_aligned<HighAlign>(48, 16)
		);

		testForResult<bool>(
			"StdExt::can_place_aligned<T>() failure parameters. (Destination is too small.)",
			false, can_place_aligned<HighAlign>(16, 8)
		);

		testForResult<bool>(
			"StdExt::can_place_aligned<T>() failure parameters. (No guarentee of enough space after alignemnt.)",
			false, can_place_aligned<HighAlign>(40, 16)
		);
	}
#	pragma endregion

#	pragma region Memory Overlaps
	{
		char mem_buffer[128];

		testForResult<bool>(
			"StdExt::memory_overlaps() success parameters. (Left starts after right.)",
			true, memory_overlaps(&mem_buffer[16], 32, &mem_buffer[0], 32)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps() success parameters. (Right starts after left.)",
			true, memory_overlaps(&mem_buffer[0], 32, &mem_buffer[16], 32)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps() success parameters. (Left encompases right.)",
			true, memory_overlaps(&mem_buffer[0], 64, &mem_buffer[16], 32)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps() success parameters. (Right encompases left.)",
			true, memory_overlaps(&mem_buffer[16], 32, &mem_buffer[0], 64)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps() non-overlap parameters.",
			false, memory_overlaps(&mem_buffer[16], 32, &mem_buffer[64], 64)
		);

		std::string str_array[32];

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Left starts after right.)",
			true,
			memory_overlaps(
				&str_array[4], 4,
				&str_array[0], 6
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Right starts after left.)",
			true,
			memory_overlaps(
				&str_array[2], 4,
				&str_array[4], 4
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Left encompases right.)",
			true, 
			memory_overlaps(
				&str_array[2], 8,
				&str_array[4], 4
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Right encompases left.)",
			true,
			memory_overlaps(
				&str_array[4], 4,
				&str_array[2], 8
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() non-overlap parameters.",
			false,
			memory_overlaps(
				&str_array[4], 4,
				&str_array[12], 8
			)
		);
	}
#	pragma endregion

#	pragma region Endianness
	{
		testForResult<uint8_t>(
			"Single byte types should not really have endianess. (uint8_t)",
			12, swap_endianness<uint8_t>(12)
		);

		testForResult<int8_t>(
			"Single byte types should not really have endianess. (int8_t)",
			-3, swap_endianness<int8_t>(-3)
		);

		testForResult<bool>(
			"Single byte types should not really have endianess. (bool)",
			true, swap_endianness<bool>(true)
		);

		testForResult<uint16_t>(
			"swap_endianness() works for two byte type (uint16_t)",
			0x3412, swap_endianness<uint16_t>(0x1234)
		);

		testForResult<int16_t>(
			"swap_endianness() works for two byte type (int16_t)",
			-5377, swap_endianness<int16_t>(-22)
		);

		testForResult<uint32_t>(
			"swap_endianness() works for four byte type (uint32_t)",
			0x78563412, swap_endianness<uint32_t>(0x12345678)
		);

		testForResult<int32_t>(
			"swap_endianness() works for four byte type (int32_t)",
			-249346713, swap_endianness<int32_t>(1732584433)
		);

		testForResult<bool>(
			"swap_endianness() works for four byte type (float32_t)",
			 true,
			 approxEqual(4735.84473f, swap_endianness<float32_t>(-127.287636f))
		);

		testForResult<uint64_t>(
			"swap_endianness() works for eight byte type (uint64_t)",
			0xE1CDAB89674523F1, swap_endianness<uint64_t>(0xF123456789ABCDE1)
		);

		testForResult<int64_t>(
			"swap_endianness() works for eight byte type (int64_t)",
			2435791580108760049, swap_endianness<int64_t>(-1070935975390360287)
		);

		testForResult<bool>(
			"swap_endianness() works for eight byte type (float64_t)",
			 true,
			 approxEqual(-12787.287635894512879, swap_endianness<float64_t>(5.524936185223521e-252))
		);

		if constexpr ( std::endian::native == std::endian::big )
		{
			testForResult<int8_t>(
				"to_little_endian() on big endian machine. (int8_t)",
				-3, to_little_endian<int8_t>(-3)
			);

			testForResult<uint8_t>(
				"to_little_endian() on big endian machine. (uint8_t)",
				12, to_little_endian<int8_t>(12)
			);

			testForResult<uint8_t>(
				"to_big_endian() on big endian machine. (uint8_t)",
				12, to_big_endian<uint8_t>(12)
			);

			testForResult<int8_t>(
				"to_big_endian() on big endian machine. (int8_t)",
				-3, to_big_endian<uint8_t>(-3)
			);

			testForResult<int16_t>(
				"to_little_endian() on big endian machine. (int16_t)",
				swap_endianness<int16_t>(-22), to_little_endian<int16_t>(-22)
			);

			testForResult<int16_t>(
				"to_big_endian() on big endian machine. (int16_t)",
				-22, to_big_endian<int16_t>(-22)
			);

			testForResult<uint16_t>(
				"to_little_endian() on big endian machine. (uint16_t)",
				swap_endianness<uint16_t>(0x1234), to_little_endian<uint16_t>(0x1234)
			);

			testForResult<uint16_t>(
				"to_big_endian() on big endian machine. (uint16_t)",
				0x1234, to_big_endian<uint16_t>(0x1234)
			);

			testForResult<int32_t>(
				"to_little_endian() on big endian machine. (int32_t)",
				swap_endianness<int32_t>(1732584433), to_little_endian<int32_t>(1732584433)
			);

			testForResult<int32_t>(
				"to_big_endian() on big endian machine. (int32_t)",
				1732584433, to_big_endian<int32_t>(1732584433)
			);

			testForResult<uint32_t>(
				"to_little_endian() on big endian machine. (uint32_t)",
				swap_endianness<uint32_t>(0x12345678), to_little_endian<uint32_t>(0x12345678)
			);

			testForResult<uint32_t>(
				"to_big_endian() on big endian machine. (uint32_t)",
				0x12345678, to_big_endian<uint32_t>(0x12345678)
			);

			testForResult<float32_t>(
				"to_little_endian() on big endian machine. (float32_t)",
				swap_endianness<float32_t>(-127.287636f), to_little_endian<float32_t>(-127.287636f)
			);

			testForResult<float32_t>(
				"to_big_endian() on big endian machine. (float32_t)",
				-127.287636f, to_big_endian<float32_t>(-127.287636f)
			);

			testForResult<int64_t>(
				"to_little_endian() on big endian machine. (int64_t)",
				swap_endianness<uint64_t>(-1070935975390360287), to_little_endian<int64_t>(-1070935975390360287)
			);

			testForResult<int64_t>(
				"to_big_endian() on big endian machine. (int64_t)",
				-1070935975390360287, to_big_endian<int64_t>(-1070935975390360287)
			);

			testForResult<uint64_t>(
				"to_little_endian() on big endian machine. (uint64_t)",
				swap_endianness<uint64_t>(0xF123456789ABCDE1), to_little_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<uint64_t>(
				"to_big_endian() on big endian machine. (uint64_t)",
				0xF123456789ABCDE1, to_big_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<float64_t>(
				"to_little_endian() on big endian machine. (float64_t)",
				swap_endianness<float64_t>(-12787.287635894512879), to_little_endian<float64_t>(-12787.287635894512879)
			);

			testForResult<float64_t>(
				"to_big_endian() on big endian machine. (float64_t)",
				-12787.287635894512879, to_big_endian<float64_t>(-12787.287635894512879)
			);

			/////////////////////////////////////////////////////////////

			testForResult<int8_t>(
				"from_little_endian() on big endian machine. (int8_t)",
				-3, from_little_endian<int8_t>(-3)
			);

			testForResult<int8_t>(
				"from_big_endian() on big endian machine. (int8_t)",
				-3, from_big_endian<int8_t>(-3)
			);

			testForResult<uint8_t>(
				"from_little_endian() on big endian machine. (uint8_t)",
				12, from_little_endian<uint8_t>(12)
			);

			testForResult<uint8_t>(
				"from_big_endian() on big endian machine. (uint8_t)",
				12, from_big_endian<uint8_t>(12)
			);

			testForResult<int16_t>(
				"from_little_endian() on big endian machine. (int16_t)",
				swap_endianness<int16_t>(-22), from_little_endian<int16_t>(-22)
			);

			testForResult<int16_t>(
				"from_big_endian() on big endian machine. (int16_t)",
				-22, from_big_endian<int16_t>(-22)
			);

			testForResult<uint16_t>(
				"from_little_endian() on big endian machine. (uint16_t)",
				swap_endianness<uint16_t>(0x1234), from_little_endian<uint16_t>(0x1234)
			);

			testForResult<uint16_t>(
				"from_big_endian() on big endian machine. (uint16_t)",
				0x1234, from_big_endian<uint16_t>(0x1234)
			);

			testForResult<int32_t>(
				"from_little_endian() on big endian machine. (int32_t)",
				swap_endianness<int32_t>(1732584433), from_little_endian<int32_t>(1732584433)
			);

			testForResult<int32_t>(
				"from_big_endian() on big endian machine. (int32_t)",
				1732584433, from_big_endian<int32_t>(1732584433)
			);

			testForResult<uint32_t>(
				"from_little_endian() on big endian machine. (uint32_t)",
				swap_endianness<uint32_t>(0x12345678), from_little_endian<uint32_t>(0x12345678)
			);

			testForResult<uint32_t>(
				"from_big_endian() on big endian machine. (uint32_t)",
				0x12345678, from_big_endian<uint32_t>(0x12345678)
			);

			testForResult<float32_t>(
				"from_little_endian() on big endian machine. (float32_t)",
				swap_endianness<float32_t>(-127.287636f), from_little_endian<float32_t>(-127.287636f)
			);

			testForResult<float32_t>(
				"from_big_endian() on big endian machine. (float32_t)",
				-127.287636f, from_big_endian<float32_t>(-127.287636f)
			);

			testForResult<int64_t>(
				"from_little_endian() on big endian machine. (int64_t)",
				swap_endianness<uint64_t>(-1070935975390360287), from_little_endian<int64_t>(-1070935975390360287)
			);

			testForResult<int64_t>(
				"from_big_endian() on big endian machine. (int64_t)",
				-1070935975390360287, from_big_endian<int64_t>(-1070935975390360287)
			);

			testForResult<uint64_t>(
				"from_little_endian() on big endian machine. (uint64_t)",
				swap_endianness<uint64_t>(0xF123456789ABCDE1), from_little_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<uint64_t>(
				"from_big_endian() on big endian machine. (uint64_t)",
				0xF123456789ABCDE1, from_big_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<float64_t>(
				"from_little_endian() on big endian machine. (float64_t)",
				swap_endianness<float64_t>(-12787.287635894512879), from_little_endian<float64_t>(-12787.287635894512879)
			);

			testForResult<float64_t>(
				"from_big_endian() on big endian machine. (float64_t)",
				-12787.287635894512879, from_big_endian<float64_t>(-12787.287635894512879)
			);
		}
		else if constexpr ( std::endian::native == std::endian::little )
		{
			testForResult<int8_t>(
				"to_little_endian() on little endian machine. (int8_t)",
				-3, to_little_endian<int8_t>(-3)
			);

			testForResult<int8_t>(
				"to_big_endian() on little endian machine. (int8_t)",
				-3, to_big_endian<int8_t>(-3)
			);

			testForResult<uint8_t>(
				"to_little_endian() on little endian machine. (uint8_t)",
				12, to_little_endian<uint8_t>(12)
			);

			testForResult<uint8_t>(
				"to_big_endian() on little endian machine. (uint8_t)",
				12, to_big_endian<uint8_t>(12)
			);

			testForResult<int16_t>(
				"to_little_endian() on little endian machine. (int16_t)",
				-22, to_little_endian<int16_t>(-22)
			);

			testForResult<int16_t>(
				"to_big_endian() on little endian machine. (int16_t)",
				swap_endianness<int16_t>(-22), to_big_endian<int16_t>(-22)
			);

			testForResult<uint16_t>(
				"to_little_endian() on little endian machine. (uint16_t)",
				0x1234, to_little_endian<uint16_t>(0x1234)
			);

			testForResult<uint16_t>(
				"to_big_endian() on little endian machine. (uint16_t)",
				swap_endianness<uint16_t>(0x1234), to_big_endian<uint16_t>(0x1234)
			);

			testForResult<int32_t>(
				"to_little_endian() on little endian machine. (int32_t)",
				1732584433, to_little_endian<int32_t>(1732584433)
			);

			testForResult<int32_t>(
				"to_big_endian() on little endian machine. (int32_t)",
				swap_endianness<int32_t>(1732584433), to_big_endian<int32_t>(1732584433)
			);

			testForResult<uint32_t>(
				"to_little_endian() on little endian machine. (uint32_t)",
				0x12345678, to_little_endian<uint32_t>(0x12345678)
			);

			testForResult<uint32_t>(
				"to_big_endian() on little endian machine. (uint32_t)",
				swap_endianness<uint32_t>(0x12345678), to_big_endian<uint32_t>(0x12345678)
			);

			testForResult<float32_t>(
				"to_little_endian() on little endian machine. (float32_t)",
				-127.287636f, to_little_endian<float32_t>(-127.287636f)
			);

			testForResult<float32_t>(
				"to_big_endian() on little endian machine. (float32_t)",
				swap_endianness<float32_t>(-127.287636f), to_big_endian<float32_t>(-127.287636f)
			);

			testForResult<int64_t>(
				"to_little_endian() on little endian machine. (int64_t)",
				-1070935975390360287, to_little_endian<int64_t>(-1070935975390360287)
			);

			testForResult<int64_t>(
				"to_big_endian() on little endian machine. (int64_t)",
				swap_endianness<uint64_t>(-1070935975390360287), to_big_endian<int64_t>(-1070935975390360287)
			);

			testForResult<uint64_t>(
				"to_little_endian() on little endian machine. (uint64_t)",
				0xF123456789ABCDE1, to_little_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<uint64_t>(
				"to_big_endian() on little endian machine. (uint64_t)",
				swap_endianness<uint64_t>(0xF123456789ABCDE1), to_big_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<float64_t>(
				"to_little_endian() on little endian machine. (float64_t)",
				-12787.287635894512879, to_little_endian<float64_t>(-12787.287635894512879)
			);

			testForResult<float64_t>(
				"to_big_endian() on little endian machine. (float64_t)",
				swap_endianness<float64_t>(-12787.287635894512879), to_big_endian<float64_t>(-12787.287635894512879)
			);

			/////////////////////////////////////////////////////////////

			testForResult<int8_t>(
				"from_little_endian() on little endian machine. (int8_t)",
				-3, from_little_endian<int8_t>(-3)
			);

			testForResult<int8_t>(
				"from_big_endian() on little endian machine. (int8_t)",
				-3, from_big_endian<int8_t>(-3)
			);

			testForResult<uint8_t>(
				"from_little_endian() on little endian machine. (uint8_t)",
				12, from_little_endian<uint8_t>(12)
			);

			testForResult<uint8_t>(
				"from_big_endian() on little endian machine. (uint8_t)",
				12, from_big_endian<uint8_t>(12)
			);

			testForResult<int16_t>(
				"from_little_endian() on little endian machine. (int16_t)",
				-22, from_little_endian<int16_t>(-22)
			);

			testForResult<int16_t>(
				"from_big_endian() on little endian machine. (int16_t)",
				swap_endianness<int16_t>(-22), from_big_endian<int16_t>(-22)
			);

			testForResult<uint16_t>(
				"from_little_endian() on little endian machine. (uint16_t)",
				0x1234, from_little_endian<uint16_t>(0x1234)
			);

			testForResult<uint16_t>(
				"from_big_endian() on little endian machine. (uint16_t)",
				swap_endianness<uint16_t>(0x1234), from_big_endian<uint16_t>(0x1234)
			);

			testForResult<int32_t>(
				"from_little_endian() on little endian machine. (int32_t)",
				1732584433, from_little_endian<int32_t>(1732584433)
			);

			testForResult<int32_t>(
				"from_big_endian() on little endian machine. (int32_t)",
				swap_endianness<int32_t>(1732584433), from_big_endian<int32_t>(1732584433)
			);

			testForResult<uint32_t>(
				"from_little_endian() on little endian machine. (uint32_t)",
				0x12345678, from_little_endian<uint32_t>(0x12345678)
			);

			testForResult<uint32_t>(
				"from_big_endian() on little endian machine. (uint32_t)",
				swap_endianness<uint32_t>(0x12345678), from_big_endian<uint32_t>(0x12345678)
			);

			testForResult<float32_t>(
				"from_little_endian() on little endian machine. (float32_t)",
				-127.287636f, from_little_endian<float32_t>(-127.287636f)
			);

			testForResult<float32_t>(
				"from_big_endian() on little endian machine. (float32_t)",
				swap_endianness<float32_t>(-127.287636f), from_big_endian<float32_t>(-127.287636f)
			);

			testForResult<int64_t>(
				"from_little_endian() on little endian machine. (int64_t)",
				-1070935975390360287, from_little_endian<int64_t>(-1070935975390360287)
			);

			testForResult<int64_t>(
				"from_big_endian() on little endian machine. (int64_t)",
				swap_endianness<uint64_t>(-1070935975390360287), from_big_endian<int64_t>(-1070935975390360287)
			);

			testForResult<uint64_t>(
				"from_little_endian() on little endian machine. (uint64_t)",
				0xF123456789ABCDE1, from_little_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<uint64_t>(
				"from_big_endian() on little endian machine. (uint64_t)",
				swap_endianness<uint64_t>(0xF123456789ABCDE1), from_big_endian<uint64_t>(0xF123456789ABCDE1)
			);

			testForResult<float64_t>(
				"from_little_endian() on little endian machine. (float64_t)",
				-12787.287635894512879, from_little_endian<float64_t>(-12787.287635894512879)
			);

			testForResult<float64_t>(
				"from_big_endian() on little endian machine. (float64_t)",
				swap_endianness<float64_t>(-12787.287635894512879), from_big_endian<float64_t>(-12787.287635894512879)
			);
		}
	}
#	pragma endregion

#	pragma region SharedPtr
	{
		const SharedPtr<Animal> animal_ptr = SharedPtr<Pug>::make();
		SharedPtr<Dog> dog_ptr = animal_ptr;

		testForResult<const Animal*>(
			"SharedPtr: Assignment references the same object.",
			animal_ptr.get(), dog_ptr.get()
		);

		testForException<std::bad_cast>(
			"SharedPtr: Upcast of incompatible object throws std::bad_cast exception.",
			[&]()
			{
				SharedPtr<Cat> base_ptr = animal_ptr;
			}
		);

		SharedPtr<int> shared_int = SharedPtr<int>::make(3);

		bool destruct_test = false;

		SharedPtr<NonVirtualBase> base_ptr = SharedPtr<NonVirtualSub>::make(&destruct_test);
		base_ptr.clear();

		testForResult<bool>(
			"SharedPtr: Base pointer of non-polymorphic subclass calls actual object's destructor.",
			destruct_test, true
		);
	}
#	pragma endregion
}
