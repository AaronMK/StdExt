#ifndef _STD_EXT_CONCURRENT_TIMER_H_
#define _STD_EXT_CONCURRENT_TIMER_H_

#include "../Signals/Event.h"

#include <chrono>

#ifdef _WIN32
#	include <agents.h>
#	include <optional>
#endif

namespace StdExt::Concurrent
{

	#ifdef _WIN32
		using SysTimer = std::optional< Concurrency::timer<void*> >;
	#else
	#	error "Timer not supported on current platform."
	#endif

	/**
	 * @brief
	 *  A timer class that runs within the constext of the system threadpool.
	 *  It is an implmentation of Event that fires on configured intervals.
	 */
	class STD_EXT_EXPORT Timer : public Signals::Event<>
	{
	private:
		SysTimer mSysTimer;
		std::chrono::milliseconds mInterval;

		friend class TimerHelper;

	public:
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
		bool isRunnning() const;

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
		 *  
		 */
		void oneShot();

		/**
		 * @brief
		 *  Stops the timer if it is running.
		 */
		void stop();
	};
}

#endif // !_STD_EXT_CONCURRENT_TIMER_H_