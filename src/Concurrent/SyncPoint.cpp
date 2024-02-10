#include <StdExt/Concurrent/SyncPoint.h>

namespace StdExt::Concurrent
{
	SyncInterface::SyncInterface()
		: wait_index(NO_INDEX), wait_state(WaitState::None)
	{
	}

	SyncInterface::~SyncInterface()
	{
	}

	SyncInterface::WaitState SyncInterface::waitState() const
	{
		return wait_state;
	}

	SyncPoint::SyncPoint()
	{
		mDestroyed = false;
	}

	SyncPoint::~SyncPoint()
	{
		destroy();
	}
}