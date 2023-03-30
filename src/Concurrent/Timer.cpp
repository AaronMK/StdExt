#include <StdExt/Concurrent/Timer.h>

#ifdef _WIN32
	using namespace Concurrency;
#else
#	include <StdExt/Concurrent/MessageLoop.h>

#	include <signal.h>
#	include <functional>
#endif

using namespace std::chrono_literals;
using namespace std::chrono;

namespace StdExt::Concurrent
{
	std::chrono::milliseconds Timer::interval() const
	{
		return mInterval;
	}

	void Timer::setInterval(std::chrono::milliseconds ms)
	{
		if ( isRunning() && ms != mInterval)
			start(ms);
		else
			mInterval = ms;
	}

	bool Timer::isRunning() const
	{
		return mSysTimer.has_value();
	}

#ifdef _WIN32
	class TimerHelper
	{
	public:
		static void doIntervalNotify(void* timer)
		{
			access_as<Timer*>(timer)->onTimeout();
		}

		
		static void doOneshotNotify(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);

			timer_ptr->onTimeout();
			timer_ptr->mSysTimer.reset();
		}
	};

	static call<void*> intervalNotify(&TimerHelper::doIntervalNotify);
	static call<void*> oneshotNotify(&TimerHelper::doOneshotNotify);
	
	Timer::Timer()
		: mInterval(0), mSysTimer{}
	{
	}

	Timer::~Timer()
	{
		stop();
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
		mSysTimer.emplace((unsigned int)mInterval.count(), this, &oneshotNotify, false);
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
#else
	static timespec fromMs(const std::chrono::milliseconds& ms)
	{
		timespec ret;
		auto floor_seconds = floor<seconds>(ms);
		auto nano_seconds = nanoseconds(ms - milliseconds(floor_seconds));

		ret.tv_sec = floor_seconds.count();
		ret.tv_nsec = nano_seconds.count();

		return ret;
	}

	static milliseconds toMs(const timespec& ts)
	{
		auto nanosecs = nanoseconds(seconds(ts.tv_sec)) + nanoseconds(ts.tv_nsec);
		return duration_cast<milliseconds>(nanosecs);
	}

	class TimerLoop : public MessageLoop< std::function<void()> >
	{
	public:
		TimerLoop()
		{
			runAsThread();
		}

		virtual ~TimerLoop()
		{
			end();
			wait();
		}

	private:
		void handleMessage(handler_param_t message) override
		{
			message();
		}
	};

	static TimerLoop timer_loop;

	class TimerHelper
	{
	public:
		static void doIntervalNotify(sigval sig)
		{
			Timer* timer_ptr = access_as<Timer*>(sig.sival_ptr);
			timer_loop.push(
				[=]()
				{
					timer_ptr->onTimeout();
				}
			);
		}
		
		static void doOneshotNotify(sigval sig)
		{
			Timer* timer_ptr = access_as<Timer*>(sig.sival_ptr);
			timer_loop.push(
				[=]()
				{
					timer_ptr->onTimeout();
				}
			);

			timer_loop.push(
				[=]()
				{
					timer_ptr->doStop();
				}
			);
		}
	};

	Timer::SysTimer::SysTimer(Timer* parent, bool one_shot)
	{
		itimerspec itspec;
		itspec.it_interval = fromMs(parent->mInterval);
		itspec.it_value = itspec.it_interval;

		sigevent sig_event;
		sig_event.sigev_notify = SIGEV_THREAD;
		sig_event.sigev_value.sival_ptr = parent;
		sig_event.sigev_notify_function = one_shot ?
			&TimerHelper::doOneshotNotify :
			&TimerHelper::doIntervalNotify;
		sig_event.sigev_notify_attributes = nullptr;

		timer_create(CLOCK_REALTIME, &sig_event, &mHandle);
		timer_settime(mHandle, 0, &itspec, nullptr);
	}
	
	Timer::SysTimer::~SysTimer()
	{
		timer_delete(mHandle);
	}

	Timer::Timer()
		: mSysTimer{}
	{
	}

	Timer::~Timer()
	{
		timer_loop.push(
			[this]()
			{
				this->doStop();
			}
		);
		timer_loop.barrier();
	}

	void Timer::start(std::chrono::milliseconds ms)
	{
		mInterval = ms;
		mSysTimer.emplace(this, false);
	}

	void Timer::start()
	{
		start(mInterval);
	}

	
	void Timer::oneShot(std::chrono::milliseconds ms)
	{
		mInterval = ms;
		mSysTimer.emplace(this, true);
	}
	
	void Timer::doStop()
	{
		if ( mSysTimer.has_value() )
		{
			mSysTimer.reset();
		}
	}

	void Timer::stop()
	{
		timer_loop.push(
			[this]()
			{
				this->doStop();
			}
		);
	}
	
#endif

}