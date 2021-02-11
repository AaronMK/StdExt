#ifndef _STD_EXT_CONCURRENT_CONDITION_H_
#define _STD_EXT_CONCURRENT_CONDITION_H_

#include "Wait.h"


#include <span>

#ifdef _WIN32

#include <concrt.h>

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
		WaitHandlePlatform mCondition;

	public:
		Condition(const Condition&) = delete;
		Condition(Condition&&) = delete;

		/**
		 * @brief
		 *  Constructor creates a condition that is in the reset state.
		 */
		Condition();
		
		/**
		 * @brief
		 *  Detroys the condition, and returns false from any wait() calls.
		 */
		virtual ~Condition();

		virtual WaitHandlePlatform* nativeWaitHandle() override;
		
		/**
		 * @brief
		 *  Blocks until the condition is triggered or detsroyed.  Returns true
		 *  if the condition was triggered, and false if it was destroyed.
		 */
		bool wait();
		
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
}
#endif // !_STD_EXT_CONCURRENT_CONDITION_H_