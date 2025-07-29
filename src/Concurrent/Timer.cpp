#include <StdExt/Concurrent/Timer.h>

#include <StdExt/Memory/Casting.h>

#if defined(STD_EXT_WIN32)
	using namespace Concurrency;
#elif defined(STD_EXT_GCC)
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
	
	Timer::Timer()
		: mInterval(0)
	{
	}

	Timer::~Timer()
	{
		stop();
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

	using recurse_lock_t = std::unique_lock<std::recursive_mutex>;

	void SysTimer::handleTimer(void* ctxt)
	{
		auto timer_context = access_as<TimerContext*>(ctxt);
		recurse_lock_t lock(timer_context->InTimer);

		if ( timer_context->ParentTimer )
			timer_context->ParentTimer->onTimeout();
	}
	
	void SysTimer::handleDestruction(void* ctxt)
	{
		auto timer_context = access_as<TimerContext*>(ctxt);

		dispatch_release(timer_context->DispatchSource);
		delete timer_context;
	}

	SysTimer::SysTimer(Timer* timer, const Chrono::Milliseconds& ms, bool one_shot)
	{
		using namespace std::chrono;

		mContext = new TimerContext();
		mContext->ParentTimer    = timer;
		mContext->DispatchSource = dispatch_source_create(
			DISPATCH_SOURCE_TYPE_TIMER, 0, 0, timer_queue
		);

		if (NULL == mContext->DispatchSource)
		{
			delete mContext;
			throw std::runtime_error("Failed to create timer;");
		}

		auto sys_timer = mContext->DispatchSource;

		dispatch_set_context(sys_timer, mContext);
		dispatch_source_set_event_handler_f(sys_timer, &handleTimer);
		dispatch_source_set_cancel_handler_f(sys_timer, &handleDestruction);

		int64_t ns_interval = duration_cast<nanoseconds>(ms).count();
		dispatch_time_t dt_start = dispatch_time(DISPATCH_TIME_NOW, ns_interval);

		uint64_t ui_interval = ( one_shot ) ?
			DISPATCH_TIME_FOREVER : static_cast<uint64_t>(ns_interval);

		dispatch_source_set_timer(sys_timer, dt_start, ui_interval, 0);
		dispatch_activate(sys_timer);
	}

	SysTimer::~SysTimer()
	{
		// If this is called in the timer handler, the lock is recursive
		// and we will mark for canellation without blocking.  If not,
		// this will prevent race conditions of the parent timer being
		// deleted while it is being handled.
		recurse_lock_t lock(mContext->InTimer);

		mContext->ParentTimer = nullptr;
		dispatch_source_cancel(mContext->DispatchSource);
	}

	Timer::Timer()
		: mInterval(0)
	{
		
	}

	Timer::~Timer()
	{
		stop();
	}

	void Timer::start(Milliseconds ms)
	{
		mInterval = ms;
		start();
	}

	void Timer::start()
	{
		mSysTimer.emplace(this, mInterval, false);
	}

	void Timer::oneShot(Milliseconds ms)
	{
		mInterval = ms;
		oneShot();
	}

	void Timer::oneShot()
	{
		mSysTimer.emplace(this, mInterval, true);
	}

	void Timer::stop()
	{
		mSysTimer.reset();
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

	class TimerHelper
	{
	public:
		static void doIntervalNotify(sigval sig)
		{
			Timer* timer_ptr = access_as<Timer*>(sig.sival_ptr);
			timer_ptr->onTimeout();
		}
		
		static void doOneshotNotify(sigval sig)
		{
			Timer* timer_ptr = access_as<Timer*>(sig.sival_ptr);
			timer_ptr->onTimeout();
			timer_ptr->mSysTimer.reset();
		}
	};

	SysTimer::SysTimer(Timer* parent, bool one_shot)
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
	
	SysTimer::~SysTimer()
	{
		timer_delete(mHandle);
	}

	Timer::Timer()
		: mSysTimer{}
	{
	}

	Timer::~Timer()
	{
		stop();
	}

	void Timer::start(Chrono::Milliseconds ms)
	{
		mInterval = ms;
		mSysTimer.emplace(this, false);
	}

	void Timer::start()
	{
		start(mInterval);
	}
	
	void Timer::oneShot(Chrono::Milliseconds ms)
	{
		mInterval = ms;
		mSysTimer.emplace(this, true);
	}

	void Timer::stop()
	{
		mSysTimer.reset();
	}

#endif

}