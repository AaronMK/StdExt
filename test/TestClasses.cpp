#include "TestClasses.h"

size_t TestBase::next_id = 0;

class Opaque
{
private:
	int mValue = 0;
};

InplaceOpaqueTest::InplaceOpaqueTest()
{
	mOpaque.setValue<Opaque>();
}

InplaceOpaqueTest::~InplaceOpaqueTest()
{
}
