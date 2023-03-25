#ifndef _STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_
#define _STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_

#include "../Concepts.h"

#ifdef _WIN32
#	include <concrt.h>
#else
#	include <mutex>
#endif

#include "Condition.h"
#include "Mutex.h"
#include "Task.h"

#include "../Collections/Vector.h"
#include "../Utility.h"
#include "../FunctionPtr.h"


#include <chrono>
#include <limits>
#include <functional>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  A manual reset condition that faclitates thread-safe interaction with protected data,
	 *  and allows specification of conditional wakeup parameters for waiting threads.
	 * 
	 * @details
	 *  This class has the following distinctions from std::condition_variable:
	 * 
	 *   - It is aware of and works with task parallel and thread pool frameworks of
	 *     the target platform.
	 * 
	 *   - Isolated invokation of predicates and actions when threads are woken up are handled
	 *     by the wait calls, making an externally managed mutex for call isolation unnecessary.
	 * 
	 *   - A destroy() function is provided allowing client code to block further wait calls,
	 *     and know that any active wait calls will have either have completed or thrown
	 *     an exception.
	 *  
	 *  The class is built to facilitate thread-safe and efficient interaction with data that
	 *  is changed by threads and then alerting threads waiting on specific changes.  This works
	 *  by the waiting threads specifying the conditions on which they would like to resume,
	 *  and what they would like to do in a thread-safe context when they resume.
	 * 
	 *  Roles of Functions
	 *  -----------------
	 * 
	 *  Operation is built on functions with the following roles.  They are passed as parameters
	 *  to trigger and wait methods of the class, Tigger and predicate functions happen
	 *  automically with respect to each other and in the same thread as wait calls.  Handler
	 *  functions will happen in the context of the waiting thread, but the condition will not
	 *  destroy itself until they have completed.
	 * 
	 *  ### Trigger Functions ###
	 *
	 *  These are function passed to trigger calls that modify object states in ways that would
	 *  potentially satisfy predicate conditions of waiting threads.  These are passed to trigger
	 *  methods of the class.  They executed within the calling thread once previous triggers and
	 *  handling have completed.  After execution, predicates for wait() calls are tested in the
	 *  same thread, and those that pass (or the first that passes depending on the trigger() call)
	 *  will have their threads signaled for wakeup.
	 * 
	 *  ### Predicate Functions ###
	 * 
	 *  These functions are used to predicate the conditions on which a thread is to be resumed.
	 *  The are evaluated either in a wait() call if the PredicatedCondition is in a triggered
	 *  state, and/or win the context of trigger() calls.  These functions should return true
	 *  if the predicate condition they are testing is satisfied. A true return will cause the
	 *  the thread of the corresponding wait call to resume, a false return will cause the 
	 *  predicate to be evaluated again on the next triegger call.
	 * 
	 *  Many times it will make since to do some actual work in this function, especially if
	 *  it avoids waiting for access to guarded resources once the waiting thread resumes.
	 *  However, these actions should be quick so they don't hold up other waiting or triggering
	 *  threads.
	 * 
	 *  This is also where reset() can occur to prevent further signaling of waiting threads
	 *  until the next trigger.
	 * 
	 *  ### Handler Functions ###
	 * 
	 *  These are functions that are called once a thread has resumed, and happen in the waiting
	 *  thread.  While this means that other actions and triggers could have happened in the time
	 *  since a predicate being satisfied and the resuming of the waiting thread, doing work in
	 *  the handler has the following benefit.
	 *
	 *  - If the condition is detroyed, it will wait for handler functions to complete before
	 *    proceeding.
	 * 
	 *  Triggered and Reset States
	 *  --------------------------
	 * 
	 *  The condition starts in the reset state.  In this state, all wait calls block and no
	 *  predicates are tested.  The trigger() calls will put the condition into the triggered 
	 *  state, and testing of predicates and signaling of waiting threads will occur until
	 *  the next reset call.
	 * 
	 *  %Condition Destruction
	 *  --------------------------
	 *  
	 *  When the condition is destroyed (or a manual destroy() call is made), any handlers
	 *  who have had their predicates satisfied will be allowed to complete.  Any other
	 *  waiting threads will have an object_destroyed exception raised.
	 */
	class PredicatedCondition
	{
	public:
		using timeout_t = Condition::wait_time_t;
		using lock_t = std::unique_lock<Mutex>;

		static constexpr size_t WAKE_MAX = std::numeric_limits<size_t>::max();

	private:
		enum class WaitState
		{
			Waiting,
			Timeout,
			Active,
			Destroyed
		};

		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		mutable Mutex mMutex;
		bool mDestroyed;

		struct WaitRecordBase
		{
			Condition condition;
			size_t wait_index = NO_INDEX;
			WaitState wait_state = WaitState::Waiting;
			bool release_on_predicate = false;

			virtual bool testPredicate() const = 0;

			virtual ~WaitRecordBase() {}
		};

		template<ReturnsType<bool> predicate_t>
		struct WaitRecord : public WaitRecordBase
		{
			const predicate_t* predicate = nullptr;

			virtual bool testPredicate() const override
			{
				return (nullptr == predicate) || (*predicate)();
			}
		};

		Collections::Vector<WaitRecordBase*, 4, false> mWaitQueue;

		void vacateTriggered(size_t index)
		{
			assert(index < mWaitQueue.size());
			
			mWaitQueue[index]->wait_index = NO_INDEX;

			if ( mWaitQueue.size() > index + 1)
			{
				mWaitQueue[index] = mWaitQueue[mWaitQueue.size() - 1];
				mWaitQueue[index]->wait_index = index;
				mWaitQueue.resize(mWaitQueue.size() - 1);
			}
			else
			{
				mWaitQueue.erase_at(index);
			}
		}

		template<ReturnsType<bool> predicate_t, ReturnsType<void> action_t>
		void wait(timeout_t timeout, const predicate_t* predicate, const action_t* action)
		{
			using namespace std::chrono;
			
			WaitRecord<predicate_t> wait_record;

			{
				lock_t lock(mMutex);

				if ( mDestroyed )
					throw object_destroyed("Wait called on destroyed PredicatedCondition.");

				if ( (nullptr == predicate || (*predicate)()) )
					return;
				
				wait_record.predicate = predicate;
				wait_record.wait_index = mWaitQueue.size();

				mWaitQueue.emplace_back(&wait_record);
			}
			
			bool wait_result = wait_record.condition.wait(timeout);

			{
				lock_t lock(mMutex);

				// Scope to make sure that vacate happens while we still have the lock.
				{
					auto vacate = finalBlock(
						[this, &wait_record]()
						{
							vacateTriggered(wait_record.wait_index);
						}
					);

					if ( WaitState::Destroyed == wait_record.wait_state )
					{
						throw object_destroyed("PredicatedCondition destroyed while waiting.");
					}

					if (!wait_result)
					{
						wait_record.wait_state = WaitState::Timeout;
						throw time_out("Wait on a PredicatedCondition timed out.");
					}

					if ( action )
						(*action)();
				}
			}
		}

	public:
		PredicatedCondition()
		{
			mDestroyed = false;
		}

		virtual ~PredicatedCondition()
		{
			destroy();
		}
		
		/**
		 * @brief
		 *  Sets the triggered state to true (if not already) after calling action, and 
		 *  wakes all waiting threads whose predictes are satisfied.
		 * 
		 * @param action
		 *  An action taken in the calling thread and done atomically with
		 *  repsect to other actions and predicates passed to wait calls.
		 */
		template<ReturnsType<void> action_t>
		void trigger(const action_t& action, size_t max_wake_count = WAKE_MAX)
		{
			lock_t lock(mMutex);
			
			if ( mDestroyed )
				throw object_destroyed("Trigger called on destroyed PredicatedCondition.");
			
			action();

			size_t wake_count = 0;

			for (size_t i = 0; i < mWaitQueue.size() && wake_count < max_wake_count; ++i)
			{
				if ( mWaitQueue[i]->wait_state == WaitState::Waiting && mWaitQueue[i]->testPredicate() )
				{
					mWaitQueue[i]->wait_state = WaitState::Active;
					mWaitQueue[i]->condition.trigger();

					++wake_count;
				}
			}
		}

		/**
		 * @brief
		 *  Sets the triggered state to true, waking all waiting threads whose
		 *  predictes are satisfied.
		 */
		void trigger(size_t max_wake_count = WAKE_MAX)
		{
			trigger( []() {}, max_wake_count );
		}
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered and for a predicate to be satisfied
		 *  before taking an action in the calling thread.
		 * 
		 * @param predicate
		 *  Used to test for a precondition for taking an action.  This can be done in the calling
		 *  thread, or in the context of trigger calls to determine which threads to awaken.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		template<ReturnsType<bool> predicate_t>
		void wait(const predicate_t& predicate, timeout_t timeout = Condition::INFINITE_WAIT)
		{
			constexpr auto do_nothing = []() {};
			wait(timeout, &predicate, &do_nothing);
		}
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered and for a predicate to be satisfied
		 *  before taking an action in the calling thread.
		 * 
		 * @param predicate
		 *  Used to test for a precondition for taking an action.  This can be done in the calling
		 *  thread, or in the context of trigger calls to determine which threads to awaken.
		 * 
		 * @param action
		 *  An action taken in the calling thread and done atomically with
		 *  repsect to other actions and predicates passed to wait calls.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		template<ReturnsType<bool> predicate_t, ReturnsType<void> action_t>
		void wait(const predicate_t& predicate, const action_t& action, timeout_t timeout = Condition::INFINITE_WAIT)
		{
			wait(timeout, &predicate, &action);
		}

		/**
		 * @brief
		 *  The number of threads currently in the wait process.
		 */
		size_t activeCount() const
		{
			lock_t lock(mMutex);
			return mWaitQueue.size();
		}

		/**
		 * @brief
		 *  Destroys the predicated condition.  Any wait calls that have not
		 *  been woken with throw a object_destroyed exception, and any in
		 *  progress will be completed before this function returns.
		 * 
		 * @note
		 *  Calling this function within a wait call to the same PredicatedCondition
		 *  will cause deadlock.
		 */
		void destroy()
		{
			{
				lock_t lock(mMutex);
				mDestroyed = true;

				for (size_t i = 0; i < mWaitQueue.size(); ++i)
				{
					mWaitQueue[i]->wait_state = WaitState::Destroyed;
					mWaitQueue[i]->condition.trigger();
				}
			}
			
			while( activeCount() != 0 )
			{
				Task::yield();
			}
		}

		/**
		 * @brief
		 *  Performs an action guarded using the same mutex used for
		 *  for access control in predicate and trigger functions, and
		 *  can be done regardless of the condition state.
		 */
		template<ReturnsType<void> action_t>
		void protectedAction(const action_t& action)
		{
			lock_t lock(mMutex);
			action();
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_