#ifndef _TEST_CLASSES_H_
#define _TEST_CLASSES_H_

class TestBase
{
	bool mValid = false;

public:
	TestBase(const TestBase&) = default;
	TestBase& operator=(const TestBase&) = default;

	TestBase()
	{
		mValid = true;
	}

	TestBase(TestBase&& other) noexcept
	{
		mValid = other.mValid;
		other.mValid = false;
	}

	virtual ~TestBase()
	{
	}

	TestBase& operator=(TestBase&& other) noexcept
	{
		mValid = other.mValid;
		other.mValid = false;

		return *this;
	}

	bool isValid() const
	{
		return mValid;
	}

	operator bool() const
	{
		return mValid;
	}
};

class TestMoveOnly : public TestBase
{
	int mMoveOnly = 1;

public:
	TestMoveOnly(const TestMoveOnly&) = delete;
	TestMoveOnly(TestMoveOnly&&) = default;

	TestMoveOnly& operator=(const TestMoveOnly&) = delete;
	TestMoveOnly& operator=(TestMoveOnly&&) = default;

	TestMoveOnly()
	{
	}
};

class TestNoCopyMove : public TestBase
{
	int mNoCopyMove = 1;

public:
	TestNoCopyMove(const TestNoCopyMove&) = delete;
	TestNoCopyMove(TestNoCopyMove&&) = default;

	TestNoCopyMove& operator=(const TestNoCopyMove&) = delete;
	TestNoCopyMove& operator=(TestNoCopyMove&&) = default;

	TestNoCopyMove()
	{
	}
};

#endif // !_TEST_CLASSES_H_