#ifndef _TEST_CLASSES_H_
#define _TEST_CLASSES_H_

#include <typeindex>

#include <StdExt/InPlace.h>

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

class Opaque;

class InplaceOpaqueTest
{
public:
	InplaceOpaqueTest();
	~InplaceOpaqueTest();

private:
	StdExt::InPlace<Opaque, 32> mOpaque;
};

class Animal
{
public:
	Animal() {}
	virtual ~Animal() {}
};

class Cat : public Animal
{
public:
	Cat() {}
	virtual ~Cat() {}
};

class Dog : public Animal
{
public:
	Dog() {}
	virtual ~Dog() {}
};

class Pug : public Dog
{
public:
	Pug() {}
	virtual ~Pug() {}
};

class NonVirtualBase
{
private:
	int a;

public:
	NonVirtualBase()
	{
		a = 0;
	}
};

class NonVirtualSub : public NonVirtualBase
{
private:
	bool* mTestBool;

public:
	NonVirtualSub(bool* destruct_test = nullptr)
		: NonVirtualBase(), mTestBool(destruct_test)
	{
		if ( mTestBool )
			*mTestBool = false;
	}

	~NonVirtualSub()
	{
		if ( mTestBool )
			*mTestBool = true;
	}
};
#endif // !_TEST_CLASSES_H_