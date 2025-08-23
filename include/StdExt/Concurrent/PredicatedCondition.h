#ifndef _STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_
#define _STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_

#include "../Concepts.h"

#ifdef _WIN32
#	include <concrt.h>
#endif

#include "Utility.h"
#include "Timer.h"

#include "../Collections/Vector.h"

#include <latch>
#include <limits>
#include <semaphore>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  A condition synchronization primitive that facilitates thread-safe interaction
	 *  with protected data, and allows specification of conditional wakeup parameters
	 *  for waiting threads.
	 * 
	 * @details
	 *  The class is built to facilitate thread-safe and efficient interaction with data that
	 *  is changed by threads and then alerting threads waiting on specific changes.  This works
	 *  by the waiting threads specifying the conditions on which they would like to resume,
	 *  and what they would like to do in a thread-safe context when they resume. It has the
	 *  following distinctions from std::condition_variable:
	 * 
	 *   - It is aware of and works with task parallel and thread pool frameworks of
	 *     the target platform.
	 * 
	 *   - Isolated invocation of predicates and actions when threads are woken up are handled
	 *     by the wait calls, making an externally managed mutex for call isolation unnecessary.
	 * 
	 *   - Many std::condition_variable implementations spin-wait on the mutex while testing
	 *     predicate conditions.  This implementation runs predicate testing within the trigger
	 *     calls to reduce spurious wakeup of waiting threads.
	 * 
	 *   - A destroy() function is provided allowing client code to block further wait calls,
	 *     and know that any active wait calls will have either have completed or thrown
	 *     an exception.
	 * 
	 *  Roles of Functions
	 *  -----------------
	 * 
	 *  Operation is built on functions with the following roles.  They are passed as parameters
	 *  to trigger and wait methods of the class, trigger and predicate functions happen
	 *  automatically with respect to each other and in the same thread as wait calls.  Handler
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
	 *  These functions should return true if the predicate condition they are testing is satisfied. 
	 *  A true return will cause the the thread of the corresponding wait call to resume,
	 *  a false return will cause the predicate to be evaluated again on the next trigger call.
	 * 
	 *  Many times it will make sense to do some actual work in this function, especially if
	 *  it avoids waiting for access to guarded resources once the waiting thread resumes.
	 *  However, these actions should be quick so they don't hold up other waiting or triggering
	 *  threads.
	 * 
	 *  ### Handler Functions ###
	 * 
	 *  These are functions that are called once a thread has resumed, and happen in the waiting
	 *  thread.  While this means that other actions and triggers could have happened in the time
	 *  since a predicate being satisfied and the resuming of the waiting thread, doing work in
	 *  the handler has the following benefit:
	 *
	 *  - If the PredicatedCondition is destroyed, the destructor will wait for handler functions
	 *    to complete before proceeding.
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
	private:
		enum class WaitState
		{
			/**
			 * @brief
			 *  Precondition has not been met and the item has not timed out.
			 */
			Waiting,

			/**
			 * @brief
			 *  Precondition has been met and the item is waiting to execute.
			 */
			Active,

			/**
			 * @brief
			 *  A timeout period for the wait has passed.
			 */
			Timeout,

			/**
			 * @brief
			 *  Item has been marked for destruction since precondition was not
			 *  met before destroy() was called.
			 */
			Destroyed
		};

		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		/**
		 * @brief
		 *  A record storing the state of a wait operation on a PredicatedCondition, and an
		 *  interface that allows it to test predicates set by client code, and wake it up
		 *  when those conditions are satisfied or when the condition object is destroyed.
		 */
		class WaitRecord
		{
		public:

			/**
			 * @brief
			 *  Index of the record in mWaitQueue, or NO_INDEX if the record is not in the
			 *  wait queue.
			 */
			size_t wait_index = NO_INDEX;

			/**
			 * @brief
			 *  Holds the semaphore as it is being passed around in waking logic.
			 */
			SemLock lock;

			/**
			 * @brief
			 *  Pointer to the next waiting record that should be woken.  This is set when
			 *  trigger is called and predicates are tested or destroy() is called.
			 */
			WaitRecord* next_to_wake = nullptr;

			/**
			 * Current state of the waiting block.
			 */
			WaitState wait_state = WaitState::Waiting;

			/**
			 * @brief
			 *  Flag that is set when the predicated is tested and satisfied.
			 */
			bool predicate_satisfied = false;
			
			/**
			 * @brief
			 *  Tests the predicate condition.  This will be called in the threads
			 *  call wait() functions, and if not satistied, then in the context of
			 *  threads that call trigger() or destroy().  Once this funtion returns
			 *  true, it will not be called again and the thread will be woken.  Must
			 *  be non-null.
			 */
			CallablePtr<bool()> testPredicate;

			/**
			 * @brief
			 *  An action that needs to happen atomically with respect to other waiters
			 *  in the context of the waiting callstack.  If this is a null pointer,
			 *  triggering code can simply wake that calling context without having to
			 *  wait for any action to finish.
			 */
			CallablePtr<void()> action;

			/**
			 * @brief
			 *  Performs logic needed to wake a waiting thread.  Must be non-null.
			 */
			CallablePtr<void()> wakeup;

			bool checkPredicate()
			{
				return (predicate_satisfied || (predicate_satisfied = testPredicate()));
			}
		};

		Collections::Vector<WaitRecord*, 4, false> mWaitQueue;
		mutable std::binary_semaphore mSemaphore{1};
		bool mDestroy{false};

		bool removeFromWait(WaitRecord* record)
		{
			size_t index = record->wait_index;

			if ( index >= mWaitQueue.size() )
				return false;

			record->wait_index = NO_INDEX;
			mWaitQueue[index] = nullptr;

			size_t end_index = mWaitQueue.size() - 1;

			if ( end_index > index )
			{
				mWaitQueue[index] = std::exchange(mWaitQueue[end_index], nullptr);
				mWaitQueue[index]->wait_index = index;
			}

			mWaitQueue.resize(end_index);
			
			return true;
		}

		/**
		 * @brief
		 *  Tests the conditions of waiting threads, and makes en execution linked list in the
		 *  wait structures so they can wake and execute their thread sensitive code in the calling
		 *  context without contention.
		 */
		WaitRecord* makeWakeChain(size_t max_wake_count = WAKE_MAX)
		{
			WaitRecord* first_to_wake = nullptr;
			WaitRecord* last_to_wake  = nullptr;
			size_t wake_count = 0;

			auto setNextToWake = [&](WaitRecord* record)
			{
				if (nullptr == first_to_wake)
					first_to_wake = record;
				else
					last_to_wake->next_to_wake = record;

				last_to_wake = record;
				++wake_count;
			};

			for(size_t idx = 0; idx < mWaitQueue.size() && wake_count < max_wake_count; ++idx)
			{
				WaitRecord* curr_record = mWaitQueue[idx];
				bool pred_satisfied = curr_record->checkPredicate();

				if ( pred_satisfied )
				{
					curr_record->wait_state = WaitState::Active;
					setNextToWake(curr_record);
				}
				else if ( mDestroy )
				{
					curr_record->wait_state = WaitState::Destroyed;
					setNextToWake(curr_record);
				}
			}

			return first_to_wake;
		}

	public:
		static constexpr size_t WAKE_MAX = std::numeric_limits<size_t>::max();

		constexpr PredicatedCondition()
		{
		}

		virtual ~PredicatedCondition()
		{
			destroy();
		}
		
		/**
		 * @brief
		 *  Sets the triggered state to true (if not already) after calling action, and 
		 *  wakes all waiting threads whose predicates are satisfied.
		 * 
		 * @param action
		 *  An action taken in the calling thread and done atomically with
		 *  respect to other actions and predicates passed to wait calls.
		 */
		template<CallableWith<void> action_t>
		void trigger(const action_t& action, size_t max_wake_count = WAKE_MAX)
		{
			SemLock lock(mSemaphore);

			if ( mDestroy )
				throw object_destroyed("Trigger called on destroyed PredicatedCondition.");

			action();

			if ( 0 == max_wake_count )
				return;

			WaitRecord* record = makeWakeChain(max_wake_count);

			if ( record )
			{
				record->lock = std::move(lock);
				record->wakeup();
			}
		}

		/**
		 * @brief
		 *  Sets the triggered state to true, waking all waiting threads whose
		 *  predicates are satisfied.
		 */
		void trigger(size_t max_wake_count = WAKE_MAX)
		{
			trigger([]() {}, max_wake_count);
		}

		void wait(CallablePtr<bool()> predicate, CallablePtr<void()> action)
		{
			std::latch continue_signal{2};
			auto wakeup = [&]()
			{
				continue_signal.count_down();
			};

			WaitRecord wait_record;
			wait_record.testPredicate = predicate;
			wait_record.wakeup = &wakeup;
			wait_record.action = action;

			{
				SemLock lock(mSemaphore);

				if ( mDestroy )
					throw object_destroyed("Wait called on destroyed PredicatedCondition.");

				if ( predicate() )
				{
					if ( action )
						action();

					return;
				}
				
				wait_record.wait_index = mWaitQueue.size();
				mWaitQueue.emplace_back(&wait_record);
			}
			
			continue_signal.arrive_and_wait();
			assert(wait_record.lock.owns_lock() || !action);

			auto vacate = finalBlock(
				[&]()
				{
					removeFromWait(&wait_record);

					if ( wait_record.next_to_wake )
					{
						wait_record.next_to_wake->lock = std::move(wait_record.lock);
						wait_record.next_to_wake->wakeup();
					}
				}
			);

			if ( WaitState::Destroyed == wait_record.wait_state )
				throw object_destroyed("PredicatedCondition destroyed while waiting.");

			if (WaitState::Timeout == wait_record.wait_state)
				throw time_out("PredicatedCondition timed out.");

			if (wait_record.action)
				wait_record.action();
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
		 *  respect to other actions and predicates passed to wait calls.
		 * 
		 * @throws
		 *  An object_destroyed exception if the condition is destroyed.
		 */
		template<CallableWith<bool> predicate_t, CallableWith<void> action_t>
		void wait(const predicate_t& predicate, const action_t& action)
		{
			wait(&predicate, &action);
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
		 * @throws
		 *  An object_destroyed exception if the condition is destroyed.
		 */
		template<CallableWith<bool> predicate_t>
		void wait(const predicate_t& predicate)
		{
			wait(&predicate, nullptr);
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
		template<CallableWith<bool> predicate_t>
		void wait(const predicate_t& predicate, Chrono::Milliseconds timeout)
		{
			wait(&predicate, nullptr, timeout);
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
		 *  respect to other actions and predicates passed to wait calls.
		 * 
		 * @param timeout
		 *  The amount of time to wait.
		 * 
		 * @throws
		 *  A time_out exception on timeouts, or an object_destroyed exception if the condition
		 *  is destroyed.
		 */
		template<CallableWith<bool> predicate_t, CallableWith<void()> action_t>
		void wait(const predicate_t& predicate, const action_t& action, Chrono::Milliseconds timeout)
		{
			wait(&predicate, &action, timeout);
		}

		void wait(CallablePtr<bool()> predicate, CallablePtr<void()> action, Chrono::Milliseconds timeout)
		{
			WaitRecord record;
			std::latch continue_signal{2};

			CallableTimer timer(
				[&]()
				{
					SemLock lock(mSemaphore);

					if (record.wait_state != WaitState::Waiting)
						return;
					
					removeFromWait(&record);
					record.wait_state = WaitState::Timeout;
					continue_signal.count_down();
				}
			);

			auto wakeup = [&]()
			{
				timer.stop();
				continue_signal.count_down();
			};

			record.testPredicate = predicate;
			record.action = action;
			record.wakeup = &wakeup;

			{
				SemLock lock(mSemaphore);

				if ( record.checkPredicate() )
				{
					if ( action )
						action();

					return;
				}
				else
				{
					mWaitQueue.emplace_back(&record);
				}
			}

			timer.oneShot(timeout);
			continue_signal.arrive_and_wait();

			auto on_exit = finalBlock(
				[&]()
				{
					removeFromWait(&record);

					if ( record.next_to_wake)
					{
						record.next_to_wake->lock = std::move(record.lock);
						record.next_to_wake->wakeup();
					}
				}
			);

			if ( WaitState::Destroyed == record.wait_state )
				throw object_destroyed("PredicatedCondition destroyed while waiting.");

			if (WaitState::Timeout == record.wait_state)
				throw time_out("PredicatedCondition timed out.");

			if (record.action)
				record.action();
		}

		/**
		 * @brief
		 *  Destroys the predicated condition.  Any wait calls that have not
		 *  been woken will throw a object_destroyed exception, and any in
		 *  progress will be completed before this function returns.
		 * 
		 * @note
		 *  Calling this function within a wait call to the same PredicatedCondition
		 *  will cause deadlock.
		 */
		void destroy()
		{
			SemLock lock(mSemaphore);
			mDestroy = true;

			auto next_to_wake = makeWakeChain();

			if ( next_to_wake )
			{
				next_to_wake->lock = std::move(lock);
				next_to_wake->wakeup();
			}

			lock.acquire(mSemaphore);
		}

		/**
		 * @brief
		 *  Performs an action guarded using the same mutex used
		 *  for access control in predicate and trigger functions, and
		 *  can be done regardless of the condition state.
		 */
		template<CallableWith<void> action_t>
		void protectedAction(const action_t& action)
		{
			SemLock lock(mSemaphore);
			action();
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_
