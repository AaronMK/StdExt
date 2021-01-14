#ifndef _TEST_CLASSES_H_
#define _TEST_CLASSES_H_

#include <typeindex>

class TestBase
{
	static size_t next_id;

	size_t mId = 0;

public:
	static void resetId()
	{
		next_id = 0;
	}

	TestBase(const TestBase&) = default;
	TestBase& operator=(const TestBase&) = default;

	TestBase()
	{
		mId = ++next_id;
	}

	TestBase(TestBase&& other) noexcept
	{
		mId = other.mId;
		other.mId = 0;
	}

	virtual ~TestBase()
	{
		mId = 0;
	}

	TestBase& operator=(TestBase&& other) noexcept
	{
		mId = other.mId;
		other.mId = 0;

		return *this;
	}

	bool isValid() const
	{
		return (0 != mId);
	}

	size_t id() const
	{
		return mId;
	}

	operator bool() const
	{
		return isValid();
	}

	virtual std::type_index typeIndex() const
	{
		return std::type_index(typeid(TestBase));
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

	virtual std::type_index typeIndex() const override
	{
		return std::type_index(typeid(TestMoveOnly));
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

	virtual std::type_index typeIndex() const override
	{
		return std::type_index(typeid(TestNoCopyMove));
	}
};

#endif // !_TEST_CLASSES_H_