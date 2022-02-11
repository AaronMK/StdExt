#include<StdExt/Utility.h>

#include <StdExt/Test/Test.h>

using namespace StdExt;
using namespace StdExt::Test;

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
}