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
	 *   - Manual reset functionality.
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
	 *  to trigger and wait methods of the class, and always happen atomically with respect to
	 *  functions passed to other method calls to the same PredicatedCondition.
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
	 *  the handler has the following benefits:
	 * 
	 *  - Like all functions passed to PredicatedCondition methods, they happen atomically
	 *    relative to each other.
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
	 *  Condition Destruction
	 *  --------------------------
	 *  
	 *  When the condition is detroyed (or a manual destroy() call is made), any handlers
	 *  who have had their predicates satisfied will be allowed to complete.  Any other
	 *  waiting threads will have an object_destroyed exception raised.
	 */
	class PredicatedCondition
	{
	private:
		enum WaitState
		{
			Waiting,
			Timout,
			Active,
			Destroyed
		};

		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		using lock_t = std::unique_lock<Mutex>;

		mutable Mutex mMutex;

		bool mActive;
		bool mDestroyed;

		struct WaitRecordBase
		{
			Condition condition;
			size_t wait_index = NO_INDEX;
			WaitState wait_state = WaitState::Waiting;

			virtual bool testPredicate() const = 0;

			virtual ~WaitRecordBase() {}
		};

		template<CallableWith<bool> predicate_t>
		struct WaitRecord : public WaitRecordBase
		{
			const predicate_t* predicate = nullptr;

			virtual bool testPredicate() const override
			{
				return (*predicate)();
			}
		};

		Collections::Vector<WaitRecordBase*, 4> mWaitQueue;

		bool processAt(size_t index)
		{
			if ( index >= mWaitQueue.size() )
				return false;

			WaitRecordBase* record = mWaitQueue[index];
			
			if ( WaitState::Waiting != record->wait_state )
				return false;

			if (mDestroyed)
			{
				record->wait_state = WaitState::Destroyed;
				record->condition.trigger();

				return false;
			}
			else if (mActive && record->testPredicate())
			{
				record->wait_state = WaitState::Active;
				record->condition.trigger();

				return true;
			}

			return false;
		}

		void vacateTriggered(size_t index)
		{
			assert(index < mWaitQueue.size());

			if ( mWaitQueue.size() > index + 1)
			{
				if ( mWaitQueue[index] )
					mWaitQueue[index]->wait_index = NO_INDEX;

				mWaitQueue[index] = mWaitQueue[mWaitQueue.size() - 1];
				mWaitQueue[index]->wait_index = index;
				mWaitQueue.resize(mWaitQueue.size() - 1);
			}
			else
			{
				mWaitQueue.erase_at(index);
			}
		}

	protected:
		
		/**
		 * 
		 */
		virtual void onWaitRegistered()
		{
		}

	public:
		using timeout_t = std::chrono::steady_clock::duration;

		PredicatedCondition()
		{
			mActive = false;
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
		 * 
		 * @returns
		 *  The number of waiting threads that were awoken.  Note: this number will not
		 *  reflect counts of awakenings as a result recursive trigger calls.
		 */
		template<CallableWith<void> action_t>
		size_t trigger(const action_t& action)
		{
			lock_t lock(mMutex);
			
			mActive = true;
			action();

			size_t clients_triggered = 0;

			for (size_t i = 0; i < mWaitQueue.size(); ++i)
			{
				if ( processAt(i) )
					++clients_triggered;

				if ( !mActive && !mDestroyed )
					break;
			}

			return clients_triggered;
		}

		/**
		 * @brief
		 *  Sets the triggered state to true, waking all waiting threads whose
		 *  predictes are satisfied.
		 * 
		 * @returns
		 *  The number of waiting threads that were awoken.  Note: this number will not
		 *  reflect counts of awakenings as a result recursive trigger calls.
		 */
		size_t trigger()
		{
			return trigger( []() {} );
		}

		bool isTriggered() const
		{
			lock_t lock(mMutex);

			return mActive;
		}

		/**
		 * @brief
		 *  Resets the triggered state of the condition, preventing waiting threads from being
		 *  awoken until a trigger function is called or the condition is destroyed.
		 */
		void reset()
		{
			lock_t lock(mMutex);
			
			mActive = false;
		}
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered and for a predicate to be satisfied
		 *  before taking an action in the calling thread.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @param predicate
		 *  Used to test for a precondition for taking an action.  This can be done in the calling
		 *  thread, or in the context of trigger calls to determine which threads to awaken.
		 * 
		 * @param action
		 *  An action taken in within the calling thread if the condition is triggered and
		 *  the predicate is met within the timeout.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		template<CallableWith<bool> predicate_t, CallableWith<void> action_t>
		void wait(timeout_t timeout, const predicate_t& predicate, const action_t& action)
		{
			using namespace std::chrono;
			
			WaitRecord<predicate_t> wait_record;

			{
				lock_t lock(mMutex);

				auto call_wait_registered = finalBlock(
					[this]()
					{
						onWaitRegistered();
					}
				);

				if ( mDestroyed )
					throw object_destroyed("Wait called on destroyed PredicatedCondition.");

				if ( mActive && predicate() )
				{
					action();
					return;
				}

				mWaitQueue.emplace_back(&wait_record);

				wait_record.predicate = &predicate;
				wait_record.wait_index = mWaitQueue.size() - 1;
			}
			
			bool wait_result = wait_record.condition.wait( duration_cast<milliseconds>(timeout) );
			
			lock_t lock(mMutex);

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
					wait_record.wait_state = WaitState::Timout;
					throw time_out("Wait on a PredicatedCondition timed out.");
				}

				action();
			}
		}
		
		/**
		 * @brief
		 *  Waits for the condition to be in a triggered state.
		 * 
		 * @throws
		 *  An object_destroyed exception if the condition is destroyed before
		 *  being triggered.
		 */
		bool wait()
		{
			wait(
				timeout_t(-1),
				[]() { return true; },
				[]() {}
			);
		}
		
		/**
		 * @brief
		 *  Waits for, at most, a specifed amount of time for the condition to be
		 *  in a triggered state.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		bool wait(timeout_t timout)
		{
			wait(
				timout,
				[]() { return true; },
				[]() {}
			);
		}
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered and for a predicate to be satisfied.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @param predicate
		 *  Used to test for a precondition for taking an action.  This can be done in the calling
		 *  thread, or in the context of trigger calls to determine which threads to awaken.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		template<CallableWith<bool> predicate_t>
		void wait(timeout_t timeout, predicate_t&& predicate)
		{
			wait(
				timeout,
				std::move(predicate),
				[]() {}
			);
		}
		
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered before taking an action in the
		 *  calling thread.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @param action
		 *  An action taken in within the calling thread if the condition is triggered 
		 *  within the timeout.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		template<CallableWith<void> action_t>
		void wait(timeout_t timeout, const action_t& action)
		{
			wait(
				timeout,
				[]() { return true; },
				action
			);
		}
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered and for a predicate to be satisfied
		 *  before returning to the calling thread.
		 * 
		 * @param predicate
		 *  Used to test for a precondition for taking an action.  This can be done in the calling
		 *  thread, or in the context of trigger calls to determine which threads to awaken.
		 */
		template<CallableWith<bool> predicate_t>
		void wait(predicate_t&& predicate)
		{
			wait(
				timeout_t(-1),
				std::move(predicate),
				[]() {}
			);
		}
		
		/**
		 * @brief
		 *  Waits for this condition to be triggered before taking an action in the calling thread.
		 * 
		 * @param action
		 *  An action taken in within the calling thread if the condition is triggered and
		 *  the predicate is met within the timeout.
		 * 
		 * @throws
		 *  An object_destroyed exception if the condition is destroyed.
		 */
		template<CallableWith<void> action_t>
		void wait(const action_t& action)
		{
			wait(
				timeout_t(-1),
				[]() { return true; },
				action
			);
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
		 *  An action taken in within the calling thread if the condition is triggered and
		 *  the predicate is met.
		 * 
		 * @throws
		 *  An object_destroyed exception if the condition is destroyed.
		 */
		template<CallableWith<bool> predicate_t, CallableWith<void> action_t>
		void wait(predicate_t&& predicate, const action_t& action)
		{
			wait(
				timeout_t(-1),
				std::move(predicate),
				action
			);
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

				mActive = false;
				mDestroyed = true;

				trigger();
			}
			
			while( activeCount() != 0 )
			{
				Task::yield();
			}
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_