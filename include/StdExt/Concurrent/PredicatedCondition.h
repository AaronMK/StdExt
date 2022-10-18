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
	 *  and allows specification of conditional wakeup parameters.
	 * 
	 * @brief
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
	 *     and know that any active wait calls will have completed before destroy returns.
	 */
	class PredicatedCondition
	{
	private:
		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		using lock_t = std::unique_lock<Mutex>;

		mutable Mutex mMutex;

		bool mActive;
		bool mDestroyed;

		std::atomic<size_t> mClientsWaitingCount;
		Condition mNoClientsWaiting;

		struct WaitRecordBase
		{
			Condition condition;
			size_t wait_index = NO_INDEX;
			bool destroyed = false;

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

		auto makeDestroyGuard()
		{
			if ( 1 == ++mClientsWaitingCount )
			{
				mNoClientsWaiting.reset();
			}

			return finalBlock(
				[this]()
				{
					if ( 0 == --mClientsWaitingCount )
						mNoClientsWaiting.trigger();
				}
			);
		}

		bool triggerAt(size_t index)
		{
			if ( index >= mWaitQueue.size() )
				return false;

			WaitRecordBase* record = mWaitQueue[index];
			if (mDestroyed)
			{
				vacateTriggered(index);

				record->destroyed = true;
				record->condition.trigger();

				return true;
			}
			else if (mActive && record->testPredicate())
			{
				vacateTriggered(index);
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
			mClientsWaitingCount = 0;
			mNoClientsWaiting.trigger();
		}

		virtual ~PredicatedCondition()
		{
			destroy();
		}

		/**
		 * @brief
		 *  Wakes one waiting client whose predicate is satisfied.  The triggered
		 *  state will be set to true to wake a waiting thread, when back to
		 *  its original state once that has been completed.
		 * 
		 * @param action
		 *  An action taken in the calling thread and done atomically with
		 *  repsect to other actions and predicates passed to wait calls.
		 * 
		 * @returns
		 *  True if a client passes its predicate and is woken up,
		 *  false otherwise.
		 */
		template<CallableWith<void> action_t>
		bool triggerOne(const action_t& action)
		{
			lock_t lock(mMutex);

			if ( mDestroyed )
				throw object_destroyed("triggerOne() called on destroyed PredicatedCondition.");

			bool old_active = mActive;
			mActive = true;

			auto reset_active = finalBlock(
				[this, old_active]()
				{
					mActive = old_active;
				}
			);

			action();

			for (size_t i = 0; i < mWaitQueue.size(); ++i)
			{
				if ( triggerAt(i) )
					return true;
			}

			return false;
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
		size_t triggerAll(const action_t& action)
		{
			lock_t lock(mMutex);
			
			mActive = true;
			action();

			size_t clients_triggered = 0;

			for (size_t i = 0; i < mWaitQueue.size(); ++i)
			{
				while ( triggerAt(i) )
					++clients_triggered;
			}

			return clients_triggered;
		}

		/**
		 * @brief
		 *  Wakes one waiting client whose predicate is satisfied.  The triggered
		 *  state will be set to true to wake a waiting thread, when back to
		 *  its original state once that has been completed.
		 * 
		 * @returns
		 *  True if a client passes its predicate and is woken up,
		 *  false otherwise.
		 */
		bool triggerOne()
		{
			return triggerOne( []() {} );
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
		size_t triggerAll()
		{
			return triggerAll( []() {} );
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

			auto destroy_guard = makeDestroyGuard();

			
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

			if (wait_record.destroyed)
			{
				throw object_destroyed("PredicatedCondition destroyed while waiting.");
			}
			else if (!wait_result)
			{
				vacateTriggered(wait_record.wait_index);
				throw time_out("Wait on a PredicatedCondition timed out.");
			}

			action();
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
		 *  The number of threads currently waiting to be woken.
		 */
		size_t waitCount() const
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
			}

			triggerAll();
			mNoClientsWaiting.wait();
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PREDICATED_CONDITION_H_