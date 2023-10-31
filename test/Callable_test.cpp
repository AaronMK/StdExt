#include <StdExt/Callable.h>

#include <StdExt/Test/Test.h>

#include <tuple>

using namespace StdExt;
using namespace StdExt::Test;

class CopyCounterCallable
{
private:
	static int mNumCopies;

public:
	CopyCounterCallable()
	{
		++mNumCopies;
	}

	~CopyCounterCallable()
	{
		--mNumCopies;
	}

	int operator()()
	{
		return mNumCopies;
	}
};
int CopyCounterCallable::mNumCopies = 0;


static int static_plus_three(int i)
{
	return i + 3;
}

void testCallable()
{
	{
		auto lambda_plus_one = [](int i)
		{
			return i + 1;
		};

		auto lambda_plus_two = [](int i)
		{
			return i + 2;
		};

		StdExt::CallableRef<int, int> call_ref;
		int result = -1;

		testForException<null_pointer>(
			"CallableRef: Attampting to call a default constructed CallableRef "
			"throws a null_pointer exception.",
			[&]()
			{
				result = call_ref(1);
			}
		);

		call_ref = lambda_plus_one;

		testForResult<int>(
			"CallableRef: Correctly calls a lambda initiated function.",
			2, call_ref(1)
		);

		call_ref = lambda_plus_two;

		testForResult<int>(
			"CallableRef: Correctly calls a different lambda initiated function "
			"after an assignment to a new function.",
			3, call_ref(1)
		);

		call_ref = &static_plus_three;

		testForResult<int>(
			"CallableRef: Correctly calls a static function after being assigned "
			"a pointer to it.",
			4, call_ref(1)
		);
	}

	{
		CopyCounterCallable copy_counter_callable;

		CallableRef<int> call_ref = copy_counter_callable;

		testForResult<int>(
			"CallableRef: Correctly calls a cal;able object, and has not "
			"made a copy of it to do so.",
			1, call_ref()
		);

		auto capture_callable = Callable(
			[copy_counter_callable]() mutable
			{
				return copy_counter_callable();
			}
		);
	}
}