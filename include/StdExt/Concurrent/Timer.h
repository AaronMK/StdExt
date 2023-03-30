#ifndef _STD_EXT_CONCURRENT_TIMER_H_
#define _STD_EXT_CONCURRENT_TIMER_H_

#include "../Signals/Event.h"

#include "Condition.h"
#include "Wait.h"

#include <chrono>

#ifdef _WIN32
#	include <agents.h>
#else
#	include <time.h>
#endif

#include <optional>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  A timer class that runs within the constext of the system threadpool.
	 *  It is an implmentation of Event that fires on configured intervals.
	 */
	class STD_EXT_EXPORT Timer
	{
	private:

		/**
		 * @brief
		 *  Condition that is triggered when the timer is not running.  This
		 *  allows client code to wait for a stop to be called or a one shot
		 *  to complete before attaching or detaching to the event.
		 */
		// Condition mNotRunning;

		friend class TimerHelper;
		std::chrono::milliseconds mInterval;

	#ifdef _WIN32
		std::optional< Concurrency::timer<void*> > mSysTimer;
	#else
		class SysTimer final
		{
			timer_t mHandle{};
		public:
			SysTimer(Timer* parent, bool one_shot);
			~SysTimer();
		};

		std::optional<SysTimer> mSysTimer;

		void doStop();
	#endif

	public:
		Timer(const Timer&) = delete;
		Timer(Timer&&) = delete;

		Timer& operator==(const Timer&) = delete;
		Timer& operator==(Timer&&) = delete;

		Timer();
		virtual ~Timer();

		/**
		 * @brief
		 *  Sets the interval of the timer.  If the timer is running with
		 *  a different interval, it will be restarted with the new interval.
		 */
		void setInterval(std::chrono::milliseconds ms);

		/**
		 * @breif
		 *  Gets the timer interval.
		 */
		std::chrono::milliseconds interval() const;

		/**
		 * @brief
		 *  Returns true if the timer is running.
		 */
		bool isRunning() const;

		/**
		 * @brief
		 *  Sets the timer intervale and starts the time.  If the timer is
		 *  already running, its interval will be updated.
		 */
		void start(std::chrono::milliseconds ms);

		/**
		 * @brief
		 *  Starts a timer that has already had its interval set.
		 */
		void start();
		
		/**
		 * @brief
		 *  Triggers the timer event once after the passed interval.
		 */
		void oneShot(std::chrono::milliseconds ms);

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