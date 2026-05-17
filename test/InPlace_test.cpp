#include <StdExt/InPlace.h>

#include "TestClasses.h"

#include <StdExt/Test/Test.h>

#include <typeindex>


using namespace StdExt;
using namespace StdExt::Test;

void testInPlace()
{
#	pragma region InPlace<T>
	{
		InplaceOpaqueTest OpaqueTest;

		using in_place_t = InPlace<TestBase, sizeof(TestBase)>;

		in_place_t in_place_1;

		testForResult<bool>(
			"Default InPlace is empty.",
			true, in_place_1.isEmpty()
		);

		testForResult<TestBase*>(
			"Default InPlace get() returns nullptr.",
			nullptr, in_place_1.get()
		);

		in_place_1.setValue<TestBase>();

		testForResult<bool>(
			"setValue() populates InPlace with a valid object.",
			true, in_place_1->isValid()
		);

		testForResult<TestMoveOnly*>(
			"Casting to a subclass of actual value fails.",
			nullptr, in_place_1.cast<TestMoveOnly>()
		);

		testForResult<bool>(
			"InPlace properly reports contents can be copied.",
			true, in_place_1.canCopy()
		);

		in_place_t in_place_2(std::move(in_place_1));

		testForResult<bool>(
			"InPlace is empty after being a move argument.",
			true, in_place_1.isEmpty()
		);

		testForResult<bool>(
			"InPlace compied from move is valid.",
			true, in_place_2->isValid()
		);

		in_place_1.setValue<TestMoveOnly>();

		testForResult<bool>(
			"InPlace properly reports contents can't be copied.",
			false, in_place_1.canCopy()
		);

		testForResult<bool>(
			"InPlace properly reports contents can be moved.",
			true, in_place_1.canMove()
		);

		testForResult<bool>(
			"Casting to a subclass of template base suceeds when content is that class.",
			true, nullptr != in_place_1.cast<TestMoveOnly>()
		);

		testForResult<TestNoCopyMove*>(
			"Casting to a sibling class of actual value fails.",
			nullptr, in_place_1.cast<TestNoCopyMove>()
		);

		testForException<invalid_operation>(
			"Assingment throws exception when right contains a non-copyable type.",
			[&]()
			{
				in_place_2 = in_place_1;
			}
		);
		
		testForResult<std::type_index>(
			"Correct type index of contents is returned.",
			std::type_index(typeid(TestMoveOnly)), in_place_1.typeIndex()
		);
	}
#	pragma endregion
}