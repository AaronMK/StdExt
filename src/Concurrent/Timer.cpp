#include <StdExt/Concurrent/Timer.h>

#include <StdExt/Memory/Casting.h>

#if defined(STD_EXT_WIN32)
	using namespace Concurrency;
#elif defined(STD_EXT_GCC)
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
	
	void SysTimer::handleTimer(Timer* timer)
	{
		timer->onTimeout();
	}

	Concurrency::call<Timer*> SysTimer::mCall(&SysTimer::handleTimer);

	SysTimer::SysTimer(Timer* timer, const Chrono::Milliseconds& ms, bool repeating)
		: Concurrency::timer<Timer*>(
			static_cast<uint32_t>(ms.count()), timer,
			&mCall, repeating
		)
	{
		start();
	}

	SysTimer::~SysTimer()
	{
		stop();
		wait_for_outstanding_async_sends();
	}

	TimerSysBase::TimerSysBase()
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

	void Timer::start(Chrono::Milliseconds ms)
	{
		mInterval = ms;
		mSysTimer.emplace(this, mInterval, true);
	}

	void Timer::start()
	{
		start(mInterval);
	}

	void Timer::oneShot(Chrono::Milliseconds ms)
	{
		mInterval = ms;
		mSysTimer.emplace(this, mInterval, false);
	}

	void Timer::oneShot()
	{
		oneShot(mInterval);
	}

	void Timer::stop()
	{
		mSysTimer.reset();
	}

#elif defined(STD_EXT_APPLE)

	static dispatch_queue_t timer_queue = 
		dispatch_queue_create("StdExt Timer Queue", DISPATCH_QUEUE_CONCURRENT);

	class TimerHelper
	{
	public:
		static void onTimer(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);
			timer_ptr->onTimeout();
		}

		static void onCanceled(void* timer)
		{
			Timer* timer_ptr = access_as<Timer*>(timer);
			dispatch_release(timer_ptr->mSysTimer);

			timer_ptr->mRunning.clear();
			timer_ptr->mRunning.notify_all();
		}

		static dispatch_source_t startDispatchSource(Timer* timer)
		{
			using namespace std::chrono;

			dispatch_source_t result = dispatch_source_create(
				DISPATCH_SOURCE_TYPE_TIMER, 0, 0, timer_queue
			);

			if (NULL == result)
				throw std::runtime_error("Failed to create timer;");

			dispatch_set_context(result, timer);
			dispatch_source_set_cancel_handler_f(result, &TimerHelper::onCanceled);
			dispatch_source_set_event_handler_f(result, &TimerHelper::onTimer);

			int64_t ns_interval = duration_cast<nanoseconds>(timer->mInterval).count();
			dispatch_time_t dt_start = dispatch_time(DISPATCH_TIME_NOW, ns_interval);

			uint64_t ui_interval = ( timer->mIsOneShot ) ?
				DISPATCH_TIME_FOREVER : static_cast<uint64_t>(ns_interval);

			dispatch_source_set_timer(result, dt_start, ui_interval, 0);
			dispatch_activate(result);

			return result;
		}
	};

	TimerSysBase::TimerSysBase()
		: mSysTimer(nullptr)
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
		return (nullptr != mSysTimer);
	}

	void Timer::start(Milliseconds ms)
	{
		mInterval = ms;
		start();
	}

	void Timer::start()
	{
		stop();

		mIsOneShot = false;
		mRunning.test_and_set();
		mSysTimer = TimerHelper::startDispatchSource(this);
	}

	void Timer::oneShot(Milliseconds ms)
	{
		mInterval = ms;
		oneShot();
	}

	void Timer::oneShot()
	{
		stop();
		
		mIsOneShot = true;
		mRunning.test_and_set();
		mSysTimer = TimerHelper::startDispatchSource(this);
	}

	void Timer::stop()
	{
		if ( mSysTimer )
		{
			dispatch_source_cancel(mSysTimer);
			mRunning.wait(true);

			mSysTimer = nullptr;
		}
	}
#else
	static timespec fromMs(const std::chrono::milliseconds& ms)
	{
		timespec ret;
		auto floor_seconds = floor<seconds>(ms);
		auto nano_seconds  = nanoseconds(ms - milliseconds(floor_seconds));

		ret.tv_sec  = floor_seconds.count();
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