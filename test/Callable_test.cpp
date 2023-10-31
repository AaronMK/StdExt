#include <StdExt/Callable.h>

#include <StdExt/Test/Test.h>

#include <tuple>

using namespace StdExt;
using namespace StdExt::Test;

class CopyCounterCallable
{
private:
	static int mNumCopies;
	int* mNumCopiesRef;

public:
	CopyCounterCallable()
		: mNumCopiesRef(&mNumCopies)
	{
		++mNumCopies;
	}

	CopyCounterCallable(const CopyCounterCallable& other)
		: mNumCopiesRef(&mNumCopies)
	{
		++mNumCopies;
	}

	CopyCounterCallable(CopyCounterCallable&& other)
		: mNumCopiesRef(&mNumCopies)
	{
	}

	~CopyCounterCallable()
	{
		--mNumCopies;
	}

	void voidFunc()
	{

	}

	void voidFuncConst() const
	{

	}

	int addCount(int i)
	{
		return *mNumCopiesRef + i;
	}

	int getCount() const
	{
		return *mNumCopiesRef;
	}

	int operator()()
	{
		return *mNumCopiesRef;
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

		auto ref_caller = [](const CallableRef<int, int>&func, int arg)
		{
			return func(arg);
		};

		CallableRef<int, int> call_ref;
		int result = -1;

		testForException<null_pointer>(
			"CallableRef: Attampting to call a default constructed CallableRef "
			"throws a null_pointer exception.",
			[&]()
			{
				result = call_ref(1);
			}
		);

		testForResult<int>(
			"CallableRef: Correctly calls a lambda initiated function.",
			2, ref_caller(lambda_plus_one, 1)
		);

		testForResult<int>(
			"CallableRef: Correctly calls a different lambda initiated function.",
			3, ref_caller(lambda_plus_two, 1)
		);

		testForResult<int>(
			"CallableRef: Correctly calls an inline static lambda.",
			4, ref_caller(
				[](int i)
				{
					return i + 3;
				},
				1
			)
		);

		CopyCounterCallable counted_callable;

		auto ref_caller_int_void = [](const CallableRef<int>& func)
		{
			return func();
		};
		
		testForResult<int>(
			"CallableRef: Correctly calls a callable object.",
			1, ref_caller_int_void(counted_callable)
		);
	}

	{
		CopyCounterCallable copy_counter_callable;

		CallableRef<int> call_ref = copy_counter_callable;

		testForResult<int>(
			"CallableRef: Correctly calls a callable object, and has not "
			"made a copy of it to do so.",
			1, call_ref()
		);

		auto capture_callable = Callable(copy_counter_callable);

		testForResult<int>(
			"Callable: Correctly calls a callable object, and has "
			"made a copy of it as a capture to do so.",
			2, capture_callable()
		);
	}
}