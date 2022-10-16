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

#include <chrono>
#include <functional>

namespace StdExt::Concurrent
{
	class PredicatedCondition
	{
	private:
		using lock_t = std::unique_lock<Mutex>;
		Mutex mMutex;

		bool mActive;
		bool mDestroyed;

		std::atomic<size_t> mClientsWaitingCount;
		Condition mNoClientsWaiting; 

		struct WaitRecord
		{
			Condition* condition;
			std::function<bool()> predicate;

			WaitRecord()
			{
				condition = nullptr;
			}

			WaitRecord(Condition* cond, std::function<bool()>&& func) noexcept
			{
				predicate = std::move(func);
				condition = cond;
			}

			WaitRecord(WaitRecord&& other) noexcept
			{
				condition = other.condition;
				other.condition = nullptr;

				predicate = std::move(other.predicate);
			}

			WaitRecord& operator=(WaitRecord&& other) noexcept
			{
				assert(nullptr == condition);

				condition = other.condition;
				other.condition = nullptr;

				predicate = std::move(other.predicate);

				return *this;
			}

			WaitRecord& operator=(const WaitRecord&) = delete;
			WaitRecord(const WaitRecord& other) = delete;
		};

		Collections::Vector<WaitRecord, 4> mWaitQueue;

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

			WaitRecord& record = mWaitQueue[index];
			if (
				( mDestroyed ) ||
				(
					mActive &&
					record.condition &&
					record.predicate()
				)
			)
			{
				auto trigger_condition = record.condition;
				record.condition = nullptr;
				record.predicate = std::function<bool()>();
				trigger_condition->trigger();

				return true;
			}

			return false;
		}

		void vacateTriggered(size_t index)
		{
			if ( mWaitQueue.size() > index + 1)
			{
				mWaitQueue[index] = std::move(
					mWaitQueue[mWaitQueue.size() - 1]
				);

				mWaitQueue.resize(mWaitQueue.size() - 1);
			}
			else
			{
				mWaitQueue.erase_at(index);
			}
		}

		template<class Rep, class Period>
		void timedWait(std::chrono::duration<Rep, Period> timeout, Condition& condition)
		{
			bool result = (timeout.count() < 0) ?
				condition.wait() :
				condition.wait(
					std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
				);

			if ( !result )
				throw time_out();
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
		 *  Wakes one waiting cleint whose predicate is satisfied.
		 * 
		 * @returns
		 *  True if a client passes its predicate and is woken up.
		 *  False otherwise.
		 */
		template<CallableWith<void> action_t>
		bool triggerOne(const action_t& action)
		{
			lock_t lock(mMutex);
			
			mActive = true;
			action();

			for (size_t i = 0; i < mWaitQueue.size(); ++i)
			{
				if ( triggerAt(i) )
				{
					vacateTriggered(i);
					return true;
				}
			}

			return false;
		}
		
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
				{
					vacateTriggered(i);
					++clients_triggered;
				}
			}

			return clients_triggered;
		}

		bool triggerOne()
		{
			return triggerOne( []() {} );
		}

		size_t triggerAll()
		{
			return triggerAll( []() {} );
		}
		
		template<CallableWith<bool> predicate_t, CallableWith<void> action_t>
		void wait(timeout_t timeout, predicate_t&& predicate, const action_t& action)
		{
			auto destroy_guard = makeDestroyGuard();
			lock_t lock(mMutex);

			auto checkDestroyed = [this]()
			{
				if ( mDestroyed )
					throw object_destroyed("Wait called on destroyed PredicatedCondition.");
			};

			checkDestroyed();

			if ( mActive && predicate() )
			{
				action();
				return;
			}

			Condition wait_signal;
			mWaitQueue.emplace_back(&wait_signal, std::move(predicate));

			lock.unlock();

			timedWait(timeout, wait_signal);

			lock.lock();

			checkDestroyed();
			action();
		}
		
		bool wait()
		{
			wait(
				timeout_t(0),
				[]() { return true; },
				[]() {}
			);
		}

		bool wait(timeout_t timout)
		{
			wait(
				timout,
				[]() { return true; },
				[]() {}
			);
		}
		
		template<CallableWith<bool> predicate_t>
		void wait(timeout_t timeout, predicate_t&& predicate)
		{
			wait(
				timeout,
				std::move(predicate),
				[]() {}
			);
		}

		template<CallableWith<void> action_t>
		void wait(timeout_t timeout, const action_t& action)
		{
			wait(
				timeout,
				[]() { return true; },
				action
			);
		}
		
		template<CallableWith<bool> predicate_t>
		void wait(predicate_t&& predicate)
		{
			wait(
				timeout_t(-1),
				std::move(predicate),
				[]() {}
			);
		}
		
		template<CallableWith<void> action_t>
		void wait(const action_t& action)
		{
			wait(
				timeout_t(-1),
				[]() { return true; },
				action
			);
		}

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