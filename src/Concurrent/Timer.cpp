#include <StdExt/Concurrent/Timer.h>

#include <StdExt/Memory/Casting.h>

#ifdef STD_EXT_WIN32
	using namespace Concurrency;
#else
#	include <StdExt/Concurrent/MessageLoop.h>

#	include <signal.h>
#	include <functional>
#endif

using namespace StdExt::Chrono;
using namespace std::chrono;

namespace StdExt::Concurrent
{
	Milliseconds Timer::interval() const
	{
		return mInterval;
	}

	void Timer::setInterval(Milliseconds ms)
	{
		if ( isRunning() && ms != mInterval)
			start(ms);
		else
			mInterval = ms;
	}

#if defined(STD_EXT_WIN32)

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
			timer_ptr->stop();
		}
	};

	static call<void*> intervalNotify(&TimerHelper::doIntervalNotify);
	static call<void*> oneshotNotify(&TimerHelper::doOneshotNotify);

	TimerSysBase::TimerSysBase()
		: mSysTimer{}
	{
	}

	TimerSysBase::~TimerSysBase()
	{

	}
	
	Timer::Timer()
		: mInterval(0)
	{
	}

	Timer::~Timer()
	{
		stop();
	}

	bool Timer::isRunning() const
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

#elif defined(STD_EXT_APPLE)

	class TimerHelper
	{
	public:
		static void onRegistered(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);
			bool old_val = timer_ptr->mRunning.test_and_set();
		}

		static void onTimer(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);
			timer_ptr->onTimeout();
		}

		static void onCanceled(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);
			timer_ptr->mRunning.clear();

			dispatch_release(timer_ptr->mSysTimer);
		}

		static void create(Timer* timer)
		{
			auto nano = duration_cast<nanoseconds>(timer->mInterval);
			auto queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);

			dispatch_source_t result = dispatch_source_create(
				DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
				dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0)
			);

			dispatch_set_context(result, timer);
			dispatch_source_set_registration_handler_f(result, &onRegistered);
			dispatch_source_set_cancel_handler_f(result, &onCanceled);
			dispatch_source_set_event_handler_f(result, &onTimer);
			dispatch_activate(result);

			if ( timer->mIsOneShot )
			{
				dispatch_after(
					dispatch_time(DISPATCH_TIME_NOW, nano.count()),
					)
			}
			else
			{
				dispatch_source_set_timer(
					result, dispatch_time(DISPATCH_TIME_NOW, nano.count()), 
					nano.count(), 0
				);
			}

			timer->mSysTimer = result;
		}
	};

	TimerSysBase::TimerSysBase()
		: mSysTimer(nullptr), mIsOneShot(false)
	{
	}

	TimerSysBase::~TimerSysBase()
	{

	}

	Timer::Timer()
		: mInterval(0)
	{
		mSysTimer = dispatch_source_create(
			DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
			dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0)
		);
		dispatch_source_set_registration_handler_f(mSysTimer, &TimerHelper::onRegistered);
		dispatch_source_set_cancel_handler_f(mSysTimer, &TimerHelper::onCanceled);
		dispatch_source_set_event_handler_f(mSysTimer, &TimerHelper::onTimer);
		dispatch_activate(mSysTimer);
	}

	Timer::~Timer()
	{
		stop();
	}

	bool Timer::isRunning() const
	{
		return (nullptr != mSysTimer);
	}

	void Timer::start(Milliseconds ms)
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
		stop();
		TimerHelper::startSysTimer(this, false);
	}

	void Timer::oneShot(Milliseconds ms)
	{
		if ( mInterval != ms )
		{
			stop();
			mInterval = ms;
		}
		
		oneShot();
	}

	void Timer::oneShot()
	{
		stop();
		TimerHelper::startSysTimer(this, true);
	}

	void Timer::stop()
	{
		if ( nullptr != mSysTimer )
		{
			dispatch_source_cancel(mSysTimer);
			mRunning.wait(false);

			mSysTimer = nullptr;
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