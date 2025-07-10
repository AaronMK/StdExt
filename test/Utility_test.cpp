#include<StdExt/Utility.h>

#include <StdExt/Test/Test.h>

using namespace StdExt;
using namespace StdExt::Test;

class AddInterface
{
public:
	virtual int add(int i) const = 0;
};

class AddOne : public AddInterface
{
public:
	int add(int i) const override
	{
		return 1 + i;
	}
};

class AddTwo : public AddInterface
{
public:
	int add(int i) const override
	{
		return 1 + 2;
	}
};

void testUtility()
{
	{
		int test_int = 0;

		try
		{
			auto final_block = finalBlock(
				[&]()
				{
					test_int = 1;
				}
			);

			throw std::exception();
		}
		catch (const std::exception&)
		{
			testForResult<int>(
				"Finally block called when exception was thrown.",
				1, test_int
			);
		}
	}

	VTable<AddInterface> add_vtable;

	add_vtable.set<AddTwo>();
	testForResult("VTable calls correct implmentation after being set.", add_vtable->add(2), 4);

	add_vtable.set<AddOne>();
	testForResult("VTable calls correct implmentation after being set.", add_vtable->add(2), 3);
}