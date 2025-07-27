#ifndef _STD_EXT_CONCURRENT_TIMER_H_
#define _STD_EXT_CONCURRENT_TIMER_H_

#include "../Concepts.h"
#include "../Platform.h"

#include "../Chrono/Duration.h"

#if defined(STD_EXT_WIN32)
#	include <agents.h>
#elif defined(STD_EXT_APPLE)
#	include <dispatch/dispatch.h>

#	include <atomic>
#	include <mutex>
#else
#	include <time.h>
#endif

#include <optional>

namespace StdExt::Concurrent
{
	class Timer;

#if defined(STD_EXT_WIN32)

	class STD_EXT_EXPORT SysTimer : public Concurrency::timer<Timer*>
	{
	private:
		static void handleTimer(Timer* timer);
		static Concurrency::call<Timer*> mCall;

	public:
		SysTimer(Timer* timer, const Chrono::Milliseconds& ms, bool one_shot);
		virtual ~SysTimer();
	};
#elif defined(STD_EXT_GCC)
	class STD_EXT_EXPORT SysTimer final
	{
		timer_t mHandle{};
	public:
		SysTimer(Timer* parent, bool one_shot);
		~SysTimer();
	};
#elif defined(STD_EXT_APPLE)
	class STD_EXT_EXPORT SysTimer final
	{
	private:
		struct TimerContext
		{
			std::recursive_mutex InTimer;
			dispatch_source_t    DispatchSource;
			Timer*               ParentTimer;
		};

		TimerContext* mContext;

		static void handleTimer(void* ctxt);
		static void handleDestruction(void* ctxt);

	public:

		SysTimer(Timer* timer, const Chrono::Milliseconds& ms, bool one_shot);
		~SysTimer();
	};
#endif

	/**
	 * @brief
	 *  A timer class that runs within the context of the system threadpool.
	 */
	class STD_EXT_EXPORT Timer
	{
	private:
		friend class SysTimer;
		friend class TimerHelper;

		Chrono::Milliseconds mInterval;
		std::optional<SysTimer> mSysTimer;

	public:
		Timer(const Timer&) = delete;
		Timer(Timer&&) = delete;

		Timer& operator==(const Timer&) = delete;
		Timer& operator==(Timer&&) = delete;

		Timer();
		virtual ~Timer();

		/**
		 * @brief
		 *  Sets the interval of the timer.
		 */
		void setInterval(Chrono::Milliseconds ms);

		/**
		 * @breif
		 *  Gets the timer interval.
		 */
		Chrono::Milliseconds interval() const;

		/**
		 * @brief
		 *  Sets the timer intervale and starts the time.  If the timer is
		 *  already running, its interval will be updated.
		 */
		void start(Chrono::Milliseconds ms);

		/**
		 * @brief
		 *  Starts a timer that has already had its interval set.
		 */
		void start();
		
		/**
		 * @brief
		 *  Triggers the timer event once after the passed interval.
		 */
		void oneShot(Chrono::Milliseconds ms);

		/**
		 * @brief
		 *   Triggers the timer event once after the previously set interval.
		 */
		void oneShot();

		/**
		 * @brief
		 *  Stops the timer if it is running.
		 */
		void stop();

	protected:

		/**
		 * @brief
		 *  Handler for timouts.
		 */
		virtual void onTimeout() = 0;
	};

	template<typename callable_t>
		requires( CallableWith<callable_t, void> && Class<callable_t> )
	class CallableTimer : public Timer
	{
	private:
		callable_t mHandler;

	public:
		CallableTimer(callable_t&& callable)
			: Timer(), mHandler( std::move(callable) )
		{
		}

		CallableTimer(const callable_t& callable)
			requires( !ReferenceType<callable_t> )
			: Timer(), mHandler( callable )
		{
		}

	protected:
		virtual void onTimeout() override
		{
			mHandler();
		}
	};
	
	template<typename callable_t>
		requires( CallableWith<callable_t, void> && Class<callable_t> )
	auto makeTimer(callable_t&& callable)
	{
		return CallableTimer<callable_t>(std::move(callable));
	}

	template<typename callable_t>
		requires( CallableWith<callable_t, void> && Class<callable_t> )
	auto makeTimer(const callable_t& callable)
	{
		return CallableTimer<callable_t>(callable);
	}
}

#endif // !_STD_EXT_CONCURRENT_TIMER_H_