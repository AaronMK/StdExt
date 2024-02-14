#include <StdExt/Tasking/SyncInterface.h>

#include <cassert>

namespace StdExt::Tasking
{
	AtomicTaskSync::AtomicTaskSync(std::atomic_flag* flag)
		: mFlag(flag)
	{
		mFlag->test_and_set();
	}

	AtomicTaskSync::~AtomicTaskSync()
	{
	}

	AtomicTaskSync::AtomicTaskSync(AtomicTaskSync&& other)
	{
		other.mFlag = mFlag;
		mFlag = nullptr;
	}

	AtomicTaskSync& AtomicTaskSync::operator=(AtomicTaskSync&& other)
	{
		other.mFlag = mFlag;
		mFlag = nullptr;

		return *this;
	}

	void AtomicTaskSync::markForSuspend()
	{
		assert(mFlag);
		mFlag->clear();
	}

	void AtomicTaskSync::wake()
	{
		assert(mFlag);
		mFlag->test_and_set();
		mFlag->notify_all();
	}

	void AtomicTaskSync::clientWait()
	{
		assert(mFlag);
		mFlag->wait(false);
	};
}