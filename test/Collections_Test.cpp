#include <StdExt/Test/Test.h>

#include <StdExt/Collections/Collections.h>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::Collections;

#include <array>
#include <vector>
#include <span>

class TestClass
{
private:
	bool mValid;

public:
	TestClass() noexcept
	{
		mValid = true;
	}

	TestClass(const TestClass& other) noexcept
	{
		mValid = other.mValid;
	}

	TestClass(TestClass&& other) noexcept
	{
		mValid = other.mValid;
		other.mValid = false;
	}

	~TestClass()
	{
		mValid = false;
	}

	TestClass& operator=(const TestClass& other) noexcept
	{
		mValid = other.mValid;
		return *this;
	}

	TestClass& operator=(TestClass&& other) noexcept
	{
		mValid = other.mValid;
		other.mValid = false;
		return *this;
	}

	bool isValid() const noexcept
	{
		return mValid;
	}

	operator bool() const noexcept
	{
		return mValid;
	}
};

void testCollections()
{
#	pragma region StdExt::Collections::move_n
	{
		std::array<TestClass, 10> test_objects{};

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
					 test_objects[5] &&
					 test_objects[6] &&
					 test_objects[7] &&
					 test_objects[8] &&
					 test_objects[9];
			}
		);
	}
#	pragma endregion
}