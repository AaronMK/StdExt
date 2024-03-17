#include <StdExt/Tasking/SyncInterface.h>

#include <cassert>

namespace StdExt::Tasking
{
	
	SyncInterface::SyncInterface()
		: wait_index(NO_INDEX), wait_state(WaitState::None)
	{
	}

	SyncInterface::~SyncInterface()
	{
	}

	WaitState SyncInterface::waitState() const
	{
		return wait_state;
	}

	
	void SyncInterface::initialize()
	{
	}

	void SyncActions::initialize()
	{
	}

	AtomicTaskSync::AtomicTaskSync()
	{
		mInternalFlag.emplace();
		mFlag = std::addressof(*mInternalFlag);
		mFlag->test_and_set();
	}
	
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

	void AtomicTaskSync::suspend()
	{
		assert(mFlag);
		mFlag->clear();
	}

	void AtomicTaskSync::wake()
	{
		assert(mFlag);
		mFlag->test_and_set();
		mFlag->notify_one();
	}

	void AtomicTaskSync::clientWait()
	{
		assert(mFlag);
		mFlag->wait(false);
		mFlag->clear();
	};
}