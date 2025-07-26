#ifndef _STD_EXT_CONCURRENT_QUEUE_H_
#define _STD_EXT_CONCURRENT_QUEUE_H_

#include "../StdExt.h"
#include "../Concepts.h"

#ifdef _WIN32
#	include <concurrent_queue.h>
#else
#	include <queue>
#	include <mutex>
#endif

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Queue with a lock-free implementation on supported platforms.
	 */
	template<typename T>
		requires MoveConstructable<T> || CopyConstructable<T>
	class Queue
	{
	private:
	
	#ifdef _WIN32
		Concurrency::concurrent_queue<T> mQueue;
	#else
		struct PlatformQueue
		{
			std::queue<T> mQueue;
			std::mutex    mMutex;
		};

		PlatformQueue mPlatQueue;
	#endif

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
		#ifdef _WIN32
			mQueue.push(item);
		#else
			std::unique_lock lock( mPlatQueue.mMutex );
			mPlatQueue.mQueue.push(item);
		#endif
		}

		/**
		 * @brief
		 *  Pushes an item onto the Queue.
		 */
		void push(T&& item)
			requires MoveConstructable<T>
		{
		#ifdef _WIN32
			mQueue.push( std::move(item) );
		#else
			std::unique_lock lock( mPlatQueue.mMutex );
			mPlatQueue.mQueue.push( std::move(item) );
		#endif
		}

		/**
		 * @brief
		 *  Attempts to pop an item from the Queue, if there is something to
		 *  de-queue, it is placed in destination and true is returned.
		 *  Otherwise, destination remains unchanged and false is returned.
		 */
		bool tryPop(T& out)
		{
			#ifdef _WIN32
				return mQueue.try_pop(out);
			#else
				std::unique_lock lock( mPlatQueue.mMutex );

				if ( !mPlatQueue.mQueue.empty() )
				{
					out = mPlatQueue.mQueue.front();
					mPlatQueue.mQueue.pop();
					return true;
				}

				return false;
			#endif
		}

		bool isEmpty()
		{
		#ifdef _WIN32
			return mQueue.empty();
		#else
			std::unique_lock lock( mPlatQueue.mMutex);
			return mPlatQueue.mQueue.empty();
		#endif
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_QUEUE_H_