#include <StdExt/Test/Test.h>

#include <StdExt/Collections/Collections.h>
#include <StdExt/Collections/Vector.h>
#include <StdExt/Memory.h>

#include "TestClasses.h"

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Collections;

#include <array>
#include <vector>
#include <span>

void testCollections()
{
#	pragma region StdExt::Collections::move_n
	{
		TestBase::resetId();
		std::array<TestBase, 10> test_objects{};

		testByCheck(
			"StdExt::Collections::move_n(T* source, T* destination, size_t amt)",
			[&test_objects]()
			{
				move_n(&test_objects[0], &test_objects[5], 5);
			},
			[&test_objects]() -> bool
			{
				return 
					!test_objects[0] &&
					!test_objects[1] &&
					!test_objects[2] &&
					!test_objects[3] &&
					!test_objects[4] &&
					 test_objects[5].id() == 1 &&
					 test_objects[6].id() == 2 &&
					 test_objects[7].id() == 3 &&
					 test_objects[8].id() == 4 &&
					 test_objects[9].id() == 5;
			}
		);

		testByCheck(
			"StdExt::Collections::move_n() memory overlaps. (Destination before source.)",
			[&test_objects]()
			{
				move_n(&test_objects[5], &test_objects[3], 5);
			},
			[&test_objects]() -> bool
			{
				return 
					!test_objects[0] &&
					!test_objects[1] &&
					!test_objects[2] &&
					 test_objects[3].id() == 1 &&
					 test_objects[4].id() == 2 &&
					 test_objects[5].id() == 3 &&
					 test_objects[6].id() == 4 &&
					 test_objects[7].id() == 5 &&
					!test_objects[8] &&
					!test_objects[9];
			}
		);

		testByCheck(
			"StdExt::Collections::move_n() memory overlaps. (Source before destination.)",
			[&test_objects]()
			{
				move_n(&test_objects[3], &test_objects[5], 5);
			},
			[&test_objects]() -> bool
			{
				return 
					!test_objects[0] &&
					!test_objects[1] &&
					!test_objects[2] &&
					!test_objects[3] &&
					!test_objects[4] &&
					 test_objects[5].id() == 1 &&
					 test_objects[6].id() == 2 &&
					 test_objects[7].id() == 3 &&
					 test_objects[8].id() == 4 &&
					 test_objects[9].id() == 5;
			}
		);

		testForException<invalid_operation>(
			"StdExt::Collections::copy_n() memory overlaps throws exception",
			[&test_objects]()
			{
				copy_n(&test_objects[5], &test_objects[3], 5);
			}
		);

		testForException<std::out_of_range>(
			"StdExt::Collections::copy_n() span range out of bounds throws exception",
			[&test_objects]()
			{
				copy_n<TestBase>(
					std::span<TestBase>(&test_objects[5], 3),
					std::span<TestBase>(&test_objects[0], 2),
					3
				);
			}
		);

		testByCheck(
			"StdExt::Collections::copy_n()",
			[&test_objects]()
			{
				copy_n(&test_objects[5], &test_objects[0], 5);
			},
			[&test_objects]() -> bool
			{
				return 
					test_objects[0].id() == 1 &&
					test_objects[1].id() == 2 &&
					test_objects[2].id() == 3 &&
					test_objects[3].id() == 4 &&
					test_objects[4].id() == 5 &&
					test_objects[5].id() == 1 &&
					test_objects[6].id() == 2 &&
					test_objects[7].id() == 3 &&
					test_objects[8].id() == 4 &&
					test_objects[9].id() == 5;
			}
		);

		testByCheck(
			"StdExt::Collections::destroy_n()",
			[&test_objects]()
			{
				destroy_n(&test_objects[8], 2);
			},
			[&test_objects]() -> bool
			{
				return 
					 test_objects[0].id() == 1 &&
					 test_objects[1].id() == 2 &&
					 test_objects[2].id() == 3 &&
					 test_objects[3].id() == 4 &&
					 test_objects[4].id() == 5 &&
					 test_objects[5].id() == 1 &&
					 test_objects[6].id() == 2 &&
					 test_objects[7].id() == 3 &&
					!test_objects[8] &&
					!test_objects[9];
			}
		);

		testByCheck(
			"StdExt::Collections::insert_n()",
			[&test_objects]()
			{
				insert_n(&test_objects[0], 6, 2, 2);
			},
			[&test_objects]() -> bool
			{
				return 
					 test_objects[0].id() == 1 &&
					 test_objects[1].id() == 2 &&
					 test_objects[2].id() == 3 &&
					 test_objects[3].id() == 4 &&
					 test_objects[4].id() == 5 &&
					 test_objects[5].id() == 1 &&
					!test_objects[6] &&
					!test_objects[7] &&
					 test_objects[8].id() == 2 &&
					 test_objects[9].id() == 3;
			}
		);

		test_objects[6] = TestBase();
		test_objects[7] = TestBase();

		testByCheck(
			"StdExt::Collections::insert_n()",
			[&test_objects]()
			{
				remove_n(&test_objects[0], 6, 2, 2);
			},
			[&test_objects]() -> bool
			{
				return 
					 test_objects[0].id() == 1 &&
					 test_objects[1].id() == 2 &&
					 test_objects[2].id() == 3 &&
					 test_objects[3].id() == 4 &&
					 test_objects[4].id() == 5 &&
					 test_objects[5].id() == 1 &&
					 test_objects[6].id() == 2 &&
					 test_objects[7].id() == 3 &&
					!test_objects[8] &&
					!test_objects[9];
			}
		);
	}
#	pragma endregion

	TestBase::resetId();

#	pragma region StdExt::Vector
	{
		Vector<TestBase, 4, 4> test_vec;

		testForResult<size_t>(
			"Default constructor creates a zero length vector.",
			0, test_vec.size()
		);

		test_vec.resize(4);

		testForResult<size_t>(
			"Resize properly resizes the vector.",
			4, test_vec.size()
		);

		testByCheck(
			"resize() constructs objects for increase in size.",
			[&test_vec]() -> bool
			{
				return 
					test_vec[0].id() == 1 &&
					test_vec[1].id() == 2 &&
					test_vec[2].id() == 3 &&
					test_vec[3].id() == 4;
			}
		);

		testForResult(
			"Contents of vector are within local storage when size is within local parameter.",
			true,
			memory_overlaps(
				&test_vec, sizeof(Vector<TestBase, 4>),
				&test_vec[3], sizeof(TestBase)
			)
		);
		test_vec.reserve(5);
		TestBase* addr_of_3 = &test_vec[3];

		testForResult(
			"Contents of vector are not local when size above local parameter reserved.",
			false,
			memory_overlaps(
				&test_vec, sizeof(Vector<TestBase, 4>),
				&test_vec[3], sizeof(TestBase)
			)
		);

		testForResult<size_t>(
			"Reserve does not resize the number of elements.",
			4, test_vec.size()
		);

		testByCheck(
			"Elements have been properly moved from local storage to non-local storage.",
			[&test_vec]() -> bool
			{
				return 
					test_vec[0].id() == 1 &&
					test_vec[1].id() == 2 &&
					test_vec[2].id() == 3 &&
					test_vec[3].id() == 4;
			}
		);

		test_vec.emplace_back();

		testForResult(
			"Contents don't move when adding and element does not go above reseved space.",
			addr_of_3, &test_vec[3]
		);

		testForResult<size_t>(
			"Adding an element changes the size.",
			5, test_vec.size()
		);

		testForResult<size_t>(
			"Added element properly constructed.",
			5, test_vec[4].id()
		);

		testByCheck(
			"Resizing below the number of elements succeeds.",
			[&]()
			{
				test_vec.resize(4);
			},
			[&]()
			{
				return (test_vec.size() == 4);
			}
		);
		
		testForResult(
			"Resizing back to local threshold makes contents local again.",
			true,
			memory_overlaps(
				&test_vec, sizeof(Vector<TestBase, 4>),
				&test_vec[3], sizeof(TestBase)
			)
		);

		testForException<std::range_error>(
			"Attempting to erase outside bounds of vector throws exception.",
			[&]()
			{
				test_vec.erase_at(2, 3);
			}
		);

		testByCheck(
			"Erasing at an index properly resizies vector and moves elements.",
			[&]()
			{
				test_vec.erase_at(1, 2);
			},
			[&]()
			{
				return 
					test_vec.size() == 2 &&
					test_vec[0].id() == 1 &&
					test_vec[1].id() == 4;
			}
		);

		testByCheck(
			"Erasing at an index properly resizes vector, moves elements, and constructs filler elements.",
			[&]()
			{
				test_vec.insert_at(1, 3);
			},
			[&]()
			{
				return 
					test_vec.size() == 5 &&
					test_vec[0].id() == 1 &&
					test_vec[1].id() == 6 &&
					test_vec[2].id() == 7 &&
					test_vec[3].id() == 8 &&
					test_vec[4].id() == 4;
			}
		);
	}
#	pragma endregion
}