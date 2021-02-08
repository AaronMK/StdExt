#ifndef _STD_EXT_CONCURRENT_QUEUE_H_
#define _STD_EXT_CONCURRENT_QUEUE_H_

#include "../StdExt.h"
#include "../Concepts.h"

#ifdef _WIN32
#	include <concurrent_queue.h>
#endif

namespace StdExt::Concurrent
{

#ifdef _WIN32
	template<typename T>
	using PlatformQueue = Concurrency::concurrent_queue<T>;
#else
#	error "Concurrent::Condition is not supported on this platform."
#endif

	/**
	 * @brief
	 *  Queue with a lock-free implementation on supported platforms.
	 */
	template<typename T>
		requires MoveConstructable<T> || CopyConstructable<T>
	class Queue
	{
	private:
		PlatformQueue<T> mQueue;

	public:
		Queue()
		{
		}

		~Queue()
		{
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(const T& item)
			requires CopyConstructable<T>
		{
			mQueue.push(item);
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(T&& item)
			requires MoveConstructable<T>
		{
			mQueue.push(std::move(item));
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue, if there is something to
		 *  de-queue, it is placed in destination and true is returned.
		 *  Otherwise, destination remains unchanged and false is returned.
		 */
		bool tryPop(T& out)
		{
			return mQueue.try_pop(out);
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_QUEUE_H_