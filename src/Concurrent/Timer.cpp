#include <StdExt/Concurrent/Timer.h>

namespace StdExt::Concurrent
{

#ifdef _WIN32
	using namespace Concurrency;

	class TimerHelper
	{
	public:
		static void callNotify(void* timer)
		{
			access_as<Timer*>(timer)->notify();
		}
	};

	static call<void*> funcCall(&TimerHelper::callNotify);

	Timer::Timer()
		: mInterval(0)
	{
	}

	Timer::~Timer()
	{
		stop();
	}

	void Timer::setInterval(std::chrono::milliseconds ms)
	{
		if ( ms != mInterval )
		{
			mInterval = ms;

			if ( isRunnning() )
			{
				stop();
				start();
			}
		}
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
			stop();
			mInterval = ms;
		}
		
		start();
	}

	void Timer::start()
	{
		mSysTimer.emplace((unsigned int)mInterval.count(), this, &funcCall, true);
		mSysTimer->start();
	}

	void Timer::oneShot(std::chrono::milliseconds ms)
	{
		mInterval = ms;
		oneShot();
	}

	void Timer::oneShot()
	{
		mSysTimer.emplace((unsigned int)mInterval.count(), this, &funcCall, false);
		mSysTimer->start();
	}

	void Timer::stop()
	{
		if ( mSysTimer.has_value() )
		{
			mSysTimer->stop();
			mSysTimer.reset();
		}
	}
#endif

}