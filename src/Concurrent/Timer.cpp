#include <StdExt/Concurrent/Timer.h>

namespace StdExt::Concurrent
{

#ifdef _WIN32
	using namespace Concurrency;

	class TimerHelper
	{
	public:
		static void doIntervalNotify(void* timer)
		{
			access_as<Timer*>(timer)->notify();
		}

		
		static void doOneshotNotify(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);

			timer_ptr->notify();
			timer_ptr->mSysTimer.reset();
			timer_ptr->mNotRunning.trigger();
		}
	};

	static call<void*> intervalNotify(&TimerHelper::doIntervalNotify);
	static call<void*> oneshotNotify(&TimerHelper::doOneshotNotify);

	Timer::Timer()
		: mInterval(0)
	{
		mNotRunning.trigger();
	}

	Timer::~Timer()
	{
		stop();
	}

	WaitHandlePlatform* Timer::nativeWaitHandle()
	{
		return mNotRunning.nativeWaitHandle();
	}

	void Timer::setInterval(std::chrono::milliseconds ms)
	{
		if ( isRunnning() && ms != mInterval)
			start(ms);
		else
			mInterval = ms;
	}

	std::chrono::milliseconds Timer::interval() const
	{
		return mInterval;
	}

	bool Timer::isRunnning() const
	{
		return mSysTimer.has_value();
	}

	void Timer::start(std::chrono::milliseconds ms)
	{
		if ( mInterval != ms )
		{
			if ( mSysTimer.has_value() )
			{
				mSysTimer->stop();
				mSysTimer.reset();
			}

			mInterval = ms;
		}
		
		start();
	}

	void Timer::start()
	{
		mNotRunning.reset();
		mSysTimer.emplace((unsigned int)mInterval.count(), this, &intervalNotify, true);
		mSysTimer->start();
	}

	void Timer::oneShot(std::chrono::milliseconds ms)
	{
		mInterval = ms;
		oneShot();
	}

	void Timer::oneShot()
	{
		mNotRunning.reset();
		mSysTimer.emplace((unsigned int)mInterval.count(), this, &oneshotNotify, false);
		mSysTimer->start();
	}

	void Timer::stop()
	{
		if ( mSysTimer.has_value() )
		{
			mSysTimer->stop();
			mSysTimer.reset();

			mNotRunning.trigger();
		}
	}

	void Timer::wait()
	{
		mNotRunning.wait();
	}
#endif

}