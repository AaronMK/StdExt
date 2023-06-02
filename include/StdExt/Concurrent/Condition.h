#ifndef _STD_EXT_CONCURRENT_CONDITION_H_
#define _STD_EXT_CONCURRENT_CONDITION_H_

#include "Wait.h"

#include <chrono>

#ifdef _WIN32
#	include <concrt.h>
#else
#	include <condition_variable>
#endif

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Abstracts a manual-reset condtion that is cooperative with the system thread
	 *  pool.
	 *
	 *  Conditions can either be in the "triggered" or "reset" state.  When triggered,
	 *  any tasks or threads waiting on the condition will be released and any wait()
	 *  calls will immediately return.  When in the reset state, wait() calls will
	 *  block until the condition is destroyed or put in the triggered state.
	 */
	class STD_EXT_EXPORT Condition : public Waitable
	{
	private:
		#ifdef _WIN32
			using PlatformCondition = concurrency::event;
		#else
			using PlatformCondition = std::atomic_flag;

			std::condition_variable mStdCondition;
			std::mutex mStdMutex;
		#endif
		
		PlatformCondition mCondition;

	public:
		using wait_time_t = std::chrono::milliseconds;
		static constexpr auto INFINITE_WAIT = wait_time_t::max();

		Condition(const Condition&) = delete;
		Condition(Condition&&) = delete;

		/**
		 * @brief
		 *  Constructor creates a condition that is in the reset state.
		 */
		Condition();
		
		/**
		 * @brief
		 *  Detroys the condition, and returns false from any wait() calls.  Behavior for waiting
		 *  threads is undefined.
		 */
		virtual ~Condition();

		virtual WaitHandlePlatform nativeWaitHandle() override;
		
		/**
		 * @brief
		 *  Blocks until the condition is triggered or destroyed.  Returns true
		 *  if the condition was triggered, and false if it was destroyed.
		 */
		bool wait();
		
		/**
		 * @brief
		 *  Blocks until the condition is triggered, destroyed, or the timeout is reached.
		 *  Returns true if the condition was triggered, and false if it was destroyed.
		 */
		bool wait(std::chrono::milliseconds timeout);
		
		/**
		 * @brief
		 *  Triggers the condition, and releases and returns from all wait calls.  Any subsequent
		 *  threads that call wait() will return until reset() is called.
		 */
		void trigger();

		/**
		 * @brief
		 *  Returns true if in the triggered state, ie if wait() would immediately return.
		 */
		bool isTriggered() const;
		
		/**
		 * @brief
		 *  Resets the condition.  After this call the condition will no longer be in
		 *  triggered state.
		 */
		void reset();
	};

	/**
	 * @brief
	 *  Waits for a given timeout for a test function to return true, using
	 *  a condition variable to determine when a retest should occur.
	 * 
	 * @note
	 *  It is advised that the test function reset() the condition when testing
	 *  to prevent repeated looping.
	 * 
	 * @param condition
	 *  The condition which when triggered, will prompt a check of the test function.
	 * 
	 * @param timeout
	 *  The amount of time to wait.
	 * 
	 * @return
	 *  True if the test function evaluates to true before the timeout, false otherwise.
	 */
	template<CallableWith<bool> test_t>
	bool conditionalTimedWait(
		Condition& condition, std::chrono::milliseconds timeout,
		const test_t& test_func
	)
	{
		using namespace std::chrono;
		auto now = steady_clock::now;
		
		auto start_time = now();

		auto time_passed = [&]()
		{
			return now() - start_time;
		};

		do
		{
			if ( test_func() )
			{
				return true;
			}
			else
			{
				auto time_remaining = 
					duration_cast<milliseconds>(timeout - time_passed());

				condition.wait(time_remaining);
			}
		}
		while (time_passed() < timeout);

		return test_func();
	}
}
#endif // !_STD_EXT_CONCURRENT_CONDITION_H_
