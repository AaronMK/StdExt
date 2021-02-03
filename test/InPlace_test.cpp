#include <StdExt/InPlace.h>
#include <StdExt/Memory.h>

#include "TestClasses.h"

#include <StdExt/Test/Test.h>

#include <typeindex>


using namespace StdExt;
using namespace StdExt::Test;

void testInPlace()
{
#	pragma region InPlaceBuffer
	{
		InPlaceBuffer<32, 4> iBuffer;

		testForResult<void*>(
			"Default construction of InPlaceBuffer is a null buffer.",
			nullptr, iBuffer.data()
		);

		testForResult<size_t>(
			"Default construction of InPlaceBuffer is zero length.",
			0, iBuffer.size()
		);

		testForResult<size_t>(
			"Default construction of InPlaceBuffer does not have alignment.",
			0, iBuffer.alignment()
		);

		testForException<std::invalid_argument>(
			"Alignment arugment that is not a power of two throws std::invalid_argument.",
			[&]()
			{
				iBuffer.resize(32, 6);
			}
		);

		iBuffer.resize(32, 4);

		testForResult<bool>(
			"Allocation of local alignment and size parameters succeeds.",
			true, (nullptr != iBuffer.data())
		);

		testForResult<bool>(
			"Allocation of local alignment and size parameters is in local storage.",
			true, iBuffer.isLocal()
		);

		testForResult<size_t>(
			"Allocation of local alignment and size parameters reports proper alignment.",
			4, iBuffer.alignment()
		);

		testForResult<size_t>(
			"Allocation of local alignment and size parameters is properly aligned.",
			0, ((size_t)iBuffer.data()) % 4
		);

		testByCheck(
			"Reallocation that results in change to non-local buffer moves data.",
			[&]()
			{
				int* iPtr = access_as<int*>(iBuffer.data());
				for (int i = 0; i < 8; ++i)
					iPtr[i] = i;

				iBuffer.realloc(64);
			},
			[&]()
			{
				int* iPtr = access_as<int*>(iBuffer.data());
				for (int i = 0; i < 8; ++i)
				{
					if ( iPtr[i] != i )
						return false;
				}

				return true;
			}
		);

		testForResult<bool>(
			"Allocation of above local size parameters is not local.",
			false, iBuffer.isLocal()
		);

		testForResult<size_t>(
			"Allocation of above local size parameters is proper size.",
			64, iBuffer.size()
		);

		testForResult<size_t>(
			"Allocation of above local size parameters is reports proper alignment.",
			4, iBuffer.alignment()
		);

		testForResult<size_t>(
			"Allocation of above local size parameters is properly aligned.",
			0, ((size_t)iBuffer.data()) % 4
		);

		testByCheck(
			"Reallocation that results in change to local buffer moves data.",
			[&]()
			{
				int* iPtr = access_as<int*>(iBuffer.data());
				for (int i = 0; i < 8; ++i)
					iPtr[i] = i + 8;

				iBuffer.realloc(32);
			},
			[&]()
			{
				int* iPtr = access_as<int*>(iBuffer.data());
				for (int i = 0; i < 8; ++i)
				{
					if ( iPtr[i] != (i + 8) )
						return false;
				}

				return true;
			}
		);

		testForResult<bool>(
			"InPlaceBuffer::canAllocLocal() correctly validates possible local parameters.",
			true, iBuffer.canAllocLocal(28, 8)
		);
	}
#	pragma endregion

#	pragma region InPlace<T>
	{
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