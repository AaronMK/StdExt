#include <StdExt/Test/Test.h>
#include <StdExt/Type.h>
#include <StdExt/Any.h>

#include "TestClasses.h"

using namespace StdExt;
using namespace StdExt::Test;

void testAny()
{
#	pragma region Any
	{
		Any any_1;

		testForResult<bool>(
			"Default InPlace is empty.",
			true, any_1.isEmpty()
		);

		any_1.setValue<TestBase>();

		testForResult<bool>(
			"setValue() populates Any with a valid object.",
			false, any_1.isEmpty()
		);

		testForResult<TestMoveOnly*>(
			"Casting to a subclass of actual value fails.",
			nullptr, any_1.cast<TestMoveOnly>()
		);

		testForResult<bool>(
			"Any properly reports contents can be copied.",
			true, any_1.canCopy()
		);

		Any any_2(std::move(any_1));

		testForResult<bool>(
			"Any is empty after being a move argument.",
			true, any_1.isEmpty()
		);

		testForResult<std::type_index>(
			"Any reports correct type_index of contents. (1)",
			Type<TestBase>::index(), any_2.typeInfo().stdIndex()
		);

		any_2.setValue<std::string>();

		testForResult<std::type_index>(
			"Any reports correct type_index of contents. (2)",
			Type<std::string>::index(), any_2.typeInfo().stdIndex()
		);

		any_2.setValue<TestMoveOnly>();

		testForResult<bool>(
			"Casting to a base class of actual value succeeds.",
			true, nullptr != any_2.cast<TestBase>()
		);

		testForResult<bool>(
			"Any properly reports contents can't be copied.",
			false, any_2.canCopy()
		);

		testForResult<bool>(
			"Any properly reports contents can be moved.",
			true, any_2.canMove()
		);

		testForException<invalid_operation>(
			"Assingment throws exception when right contains a non-copyable type.",
			[&]()
			{
				any_1 = any_2;
			}
		);

		any_1.setValue<int>(1);

		testForResult<std::type_index>(
			"Any reports correct type_index of primitive contents.",
			Type<int>::index(), any_1.typeInfo().stdIndex()
		);

		testForResult<int>(
			"Any will correctly cast and store a primitive value.",
			1, *any_1.cast<int>()
		);

		testForResult<float*>(
			"Any will not cast across primitives.",
			nullptr, any_1.cast<float>()
		);

		int int_data = 2;
		any_1.setValue<int*>(&int_data);

		testForResult<int*>(
			"Any will correctly cast and store a pointer value.",
			&int_data, *any_1.cast<int*>()
		);

		testForResult<float**>(
			"Any will not cast across primitive pointer types.",
			nullptr, any_1.cast<float*>()
		);
	}
#	pragma endregion
}