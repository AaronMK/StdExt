#include <StdExt/Concurrent/Condition.h>

#include <StdExt/Exceptions.h>
#include <StdExt/Number.h>

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

	WaitHandlePlatform Condition::nativeWaitHandle()
	{
		return &mCondition;
	}

	bool Condition::wait()
	{
		return (0 == mCondition.wait(Concurrency::COOPERATIVE_TIMEOUT_INFINITE));
	}

	bool Condition::wait(std::chrono::milliseconds timout)
	{
		if ( timout.count() <= 0 )
			return (0 == mCondition.wait(0));

		return (0 == mCondition.wait(
				Number::convert<unsigned int>(timout.count())
			)
		);
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
#else
namespace StdExt::Concurrent
{
	Condition::Condition()
	{
		mCondition.clear();
	}

	Condition::~Condition()
	{
		trigger();
	}

	WaitHandlePlatform Condition::nativeWaitHandle()
	{
		return &mCondition;
	}

	bool Condition::wait()
	{
		mCondition.wait(false);
		return true;
	}

	bool Condition::wait(std::chrono::milliseconds timout)
	{
		if ( !isTriggered() )
		{
			std::unique_lock lock(mStdMutex);
			auto result = mStdCondition.wait_for(lock, timout);

			return (std::cv_status::no_timeout == result);
		}

		return true;
	}

	void Condition::trigger()
	{
		{
			std::lock_guard lock(mStdMutex);
		}
		mStdCondition.notify_all();

		if ( !mCondition.test_and_set() )
		{
			mCondition.notify_all();
		}
	}

	bool Condition::isTriggered() const
	{
		return mCondition.test();
	}

	void Condition::reset()
	{
		mCondition.clear();
	}
}
#endif