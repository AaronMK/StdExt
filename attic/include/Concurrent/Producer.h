#ifndef _STD_EXT_CONCURRENT_PRODUCER_H_
#define _STD_EXT_CONCURRENT_PRODUCER_H_

#include "../StdExt.h"
#include "../Concepts.h"

#include "PredicatedCondition.h"
#include "RWLock.h"
#include "Queue.h"

#include <queue>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Implements the producer-consumer pattern.
	 */
	template<typename T>
	class Producer
	{
		PredicatedCondition mWaitManager;
		std::queue<T> mMsgQueue;

	public:
		using timeout_t = PredicatedCondition::timeout_t;
		static constexpr auto INFINITE_WAIT = Condition::INFINITE_WAIT;
		
		Producer(const Producer&) = delete;
		Producer(Producer&&) = default;

		Producer()
		{

		}

		~Producer()
		{
			mWaitManager.destroy();
		}

		/**
		 * @brief
		 *  Constructs an item T for insertion into the queue using the passed arguments.
		 *  Either the next call to consume() or a currently waiting consume call will
		 *  be signaled.
		 */
		template<typename... args_t>
		void push(args_t ...args)
		{
			mWaitManager.trigger(
				[&]()
				{
					mMsgQueue.emplace(std::forward<args_t>(args)...);
				}, 1
			);
		}

		/**
		 * @brief
		 *  Attempts to take an item from the queue. This will return instantly.
		 *  If not, this will block either waiting for an item or for the producer
		 *  to end, either through destruction or a call to end().
		 */
		void consume(T& out, timeout_t timeout = INFINITE_WAIT)
		{
			auto tryConsume = [&]() -> bool
			{
				if (mMsgQueue.empty())
					return false;

				out = std::move( mMsgQueue.front() );
				mMsgQueue.pop();

				return true;
			};

			try
			{
				mWaitManager.wait(tryConsume, timeout);
			}
			catch(const object_destroyed&)
			{	
				bool success = false;

				mWaitManager.protectedAction(
					[&]()
					{
						success = tryConsume();
					}
				);

				if ( !success )
					throw;
			}
		}

		/**
		 * @brief
		 *  Ends the producer.  Any consume() calls waiting will be woken, and consume
		 *  remaining items.  If there are not enough consumers to do so, the remaining
		 *  Items will be destroyed.  If there are too many consume() calls, the
		 *  remaining will fail.
		 */
		void end()
		{
			mWaitManager.destroy();
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PRODUCER_H_
