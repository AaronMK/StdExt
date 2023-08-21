#ifndef _STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_
#define _STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_

#include "../Concepts.h"

#ifdef _WIN32
#	include <concrt.h>
#else
#	include <mutex>
#endif

#include "Condition.h"
#include "Timer.h"
#include "Mutex.h"
#include "Task.h"

#include "../Collections/Vector.h"
#include "../Utility.h"
#include "../FunctionPtr.h"


#include <chrono>
#include <limits>
#include <ranges>
#include <functional>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  A condition synchronization primitive that faclitates thread-safe interaction
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
	 *   - Isolated invokation of predicates and actions when threads are woken up are handled
	 *     by the wait calls, making an externally managed mutex for call isolation unnecessary.
	 * 
	 *   - Many std::condition_variable implmentations spin-wait on the mutex while testing
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
	 *  - If the PredicatedCondition is detroyed, the destructor will wait for handler functions
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
	public:
		using timeout_t = Condition::wait_time_t;
		using lock_t = std::unique_lock<Mutex>;

		static constexpr size_t WAKE_MAX = std::numeric_limits<size_t>::max();

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
			 *  Item has been marked for destruction since precondition was not
			 *  met before destroy() was called.
			 */
			Destroyed
		};

		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		struct WaitRecordBase
		{
			Condition condition;
			bool predicate_satisfied = false;
			size_t wait_index = NO_INDEX;
			WaitState wait_state = WaitState::Waiting;
			WaitRecordBase* next_to_wake = nullptr;

			virtual bool testPredicate() const = 0;
		};

		template<ReturnsType<bool> predicate_t>
		struct WaitRecord : public WaitRecordBase
		{
			const predicate_t* predicate = nullptr;

			virtual bool testPredicate() const override
			{
				assert( nullptr != predicate);
				return (*predicate)();
			}
		};

		Collections::Vector<WaitRecordBase*, 4, false> mWaitQueue;
		mutable Mutex mMutex;
		bool mDestroyed;

		bool predicateSatisfied(WaitRecordBase* record)
		{
			record->predicate_satisfied = 
				( record->predicate_satisfied || record->testPredicate() );

			return record->predicate_satisfied;
		}

		bool removeFromWait(WaitRecordBase* record)
		{
			size_t index = record->wait_index;

			if ( index >= mWaitQueue.size() )
				return false;

			record->wait_index = NO_INDEX;
			mWaitQueue[index] = nullptr;

			size_t end_index = mWaitQueue.size() - 1;

			if ( end_index > index )
			{
				mWaitQueue[index] = mWaitQueue[end_index];
				mWaitQueue[index]->wait_index = index;

				mWaitQueue[end_index] = nullptr;
			}

			mWaitQueue.resize(end_index);
			
			return true;
		}

		WaitRecordBase* makeWakeChain(size_t max_wake_count = WAKE_MAX)
		{
			auto nextToWake = [&](size_t index) -> WaitRecordBase*
			{
				for (size_t i = index; i < mWaitQueue.size(); ++i )
				{
					WaitRecordBase* record = mWaitQueue[i];

					if ( WaitState::Waiting != record->wait_state )
						continue;

					if ( mDestroyed || predicateSatisfied(record) )
					{
						record->wait_state = ( mDestroyed ) ?
							WaitState::Destroyed : WaitState::Active;

						return record;
					}
				}

				return nullptr;
			};
			
			if ( 0 == max_wake_count )
				return nullptr;

			WaitRecordBase* first_to_wake = nextToWake(0);
			WaitRecordBase* last_to_wake = first_to_wake;

			--max_wake_count;

			while ( last_to_wake && max_wake_count > 0 )
			{
				last_to_wake->next_to_wake = nextToWake(last_to_wake->wait_index + 1);
				last_to_wake = last_to_wake->next_to_wake;

				--max_wake_count;
			}

			return first_to_wake;
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
		template<CallableWith<void> action_t>
		void trigger(const action_t& action, size_t max_wake_count = WAKE_MAX)
		{
			lock_t lock(mMutex);

			if ( mDestroyed )
				throw object_destroyed("Trigger called on destroyed PredicatedCondition.");

			action();

			if ( 0 == max_wake_count )
				return;

			WaitRecordBase* record = makeWakeChain(max_wake_count);

			if ( record )
			{
				lock.unlock();
				record->condition.trigger();
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
		 * @param action
		 *  An action taken in the calling thread and done atomically with
		 *  repsect to other actions and predicates passed to wait calls.
		 * 
		 * @throws
		 *  An object_destroyed exception if the condition is destroyed.
		 */
		template<CallableWith<bool> predicate_t, CallableWith<void> action_t>
		void wait(const predicate_t& predicate, const action_t& action)
		{
			WaitRecord<predicate_t> wait_record;

			{
				lock_t lock(mMutex);

				if ( mDestroyed )
					throw object_destroyed("Wait called on destroyed PredicatedCondition.");

				if ( predicate() )
				{
					action();

					return;
				}
				
				wait_record.predicate = &predicate;
				wait_record.wait_index = mWaitQueue.size();

				mWaitQueue.emplace_back(&wait_record);
			}
			
			wait_record.condition.wait();

			auto vacate = finalBlock(
				[&, lock = lock_t(mMutex)]() mutable
				{
					removeFromWait(&wait_record);

					if ( wait_record.next_to_wake )
					{
						lock.unlock();
						wait_record.next_to_wake->condition.trigger();
					}
				}
			);
				
			if ( WaitState::Destroyed == wait_record.wait_state )
				throw object_destroyed("PredicatedCondition destroyed while waiting.");
				
			action();
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
			constexpr auto do_nothing = []() {};
			wait(predicate, do_nothing);
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
		void wait(const predicate_t& predicate, timeout_t timeout)
		{
			bool timed_out = false;

			auto timer = makeTimer(
				[&]()
				{
					try
					{
						trigger(
							[&]()
							{
								timed_out = true;
							}
						);
					}
					catch (const object_destroyed&)
					{
					}
				}
			);
			
			timer.oneShot(timeout);

			wait(
				[&]()
				{
					return (timed_out || predicate());
				},
				[&]()
				{
					if ( timed_out )
						throw time_out("Wait on a PredicatedCondition timed out.");
				}
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
		template<CallableWith<bool> predicate_t, CallableWith<void> action_t>
		void wait(const predicate_t& predicate, const action_t& action, timeout_t timeout)
		{
			bool timed_out = false;

			auto timer = makeTimer(
				[&]()
				{
					trigger(
						[&]()
						{
							timed_out = true;
						}
					);
				}
			);
			
			timer.oneShot(timeout);

			wait(
				[&]()
				{
					return (timed_out || predicate());
				},
				[&]()
				{
					if ( timed_out )
						throw time_out("Wait on a PredicatedCondition timed out.");

					action();
				}
			);
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
			{
				lock_t lock(mMutex);

				mDestroyed = true;

				auto record = makeWakeChain();

				if ( record )
				{
					lock.unlock();
					record->condition.trigger();
				}
			}
		
			auto activeCount = [&]()
			{
				lock_t lock(mMutex);
				return mWaitQueue.size();
			};
			
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
		template<CallableWith<void> action_t>
		void protectedAction(const action_t& action)
		{
			lock_t lock(mMutex);
			action();
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_