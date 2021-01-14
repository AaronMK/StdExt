#include <StdExt/Memory.h>
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
				span<string>(&str_array[4], 4),
				span<string>(&str_array[0], 6)
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Right starts after left.)",
			true,
			memory_overlaps(
				span<string>(&str_array[2], 4),
				span<string>(&str_array[4], 4)
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Left encompases right.)",
			true, 
			memory_overlaps(
				span<string>(&str_array[2], 8),
				span<string>(&str_array[4], 4)
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() success parameters. (Right encompases left.)",
			true,
			memory_overlaps(
				span<string>(&str_array[4], 4),
				span<string>(&str_array[2], 8)
			)
		);

		testForResult<bool>(
			"StdExt::memory_overlaps<T>() non-overlap parameters.",
			false,
			memory_overlaps(
				span<string>(&str_array[4], 4),
				span<string>(&str_array[12], 8)
			)
		);
	}
#	pragma endregion

}