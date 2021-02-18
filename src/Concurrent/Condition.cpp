#include <StdExt/Concurrent/Condition.h>

#include <StdExt/Collections/Vector.h>

using namespace StdExt::Collections;

#ifdef _WIN32

using namespace Concurrency;

namespace StdExt::Concurrent
{
	Condition::Condition()
	{
	}

	Condition::~Condition()
	{
		trigger();
	}

	WaitHandlePlatform* Condition::nativeWaitHandle()
	{
		return &mCondition;
	}

	bool Condition::wait()
	{
		return (0 == mCondition.wait(Concurrency::COOPERATIVE_TIMEOUT_INFINITE));
	}

	void Condition::trigger()
	{
		mCondition.set();
	}

	bool Condition::isTriggered() const
	{
		Condition* nonConstThis = const_cast<Condition*>(this);
		return (0 == nonConstThis->mCondition.wait(0));
	}

	void Condition::reset()
	{
		mCondition.reset();
	}
}

#endif