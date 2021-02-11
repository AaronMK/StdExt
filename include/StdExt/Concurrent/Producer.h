#ifndef _STD_EXT_CONCURRENT_PRODUCER_H_
#define _STD_EXT_CONCURRENT_PRODUCER_H_

#include "../StdExt.h"
#include "../Concepts.h"

#include "Condition.h"
#include "RWLock.h"
#include "Queue.h"

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Implemenets the producer-consumer pattern.
	 */
	template<typename T>
	class Producer
	{
		struct ControlStruct
		{
			Queue<T> messages;
			Queue<Condition*> waiting;

			std::atomic<bool> endCalled = false;
			RWLock rwLock;
		};

		std::shared_ptr<ControlStruct> mCtrlStruct = std::make_shared<ControlStruct>();

	public:
		Producer(const Producer&) = delete;
		Producer(Producer&&) = default;

		Producer()
		{

		}

		~Producer()
		{
			end();
		}

		/**
		 * @brief
		 *  Pushes an item into the queue for consumption.  If consume calls
		 *  are blocked, one will be woken to consume the item.
		 */
		void push(const T& item)
		{
			std::shared_ptr<ControlStruct> ctrl = mCtrlStruct;

			WriteLocker lock(ctrl->rwLock);
			ctrl->messages.push(item);

			Condition* cond = nullptr;
			if ( !ctrl->messages.isEmpty() && ctrl->waiting.tryPop(cond) )
				cond->trigger();
		}

		/**
		 * @brief
		 *  Pushes an item into the queue for consumption.  If consume calls
		 *  are blocked, one will be woken to consume the item.
		 */
		void push(T&& item)
		{
			std::shared_ptr<ControlStruct> ctrl = mCtrlStruct;

			WriteLocker lock(ctrl->rwLock);
			ctrl->messages.push(std::move(item));

			Condition* cond = nullptr;
			if ( !ctrl->messages.isEmpty() && ctrl->waiting.tryPop(cond) )
				cond->trigger();
		}

		/**
		 * @brief
		 *  Attempts to take an item from the queue. This will return instantly.
		 *  If not, this will block either waiting for an item or for the produecer
		 *  to end, either through destruction or a call to end().
		 */
		bool consume(T& out)
		{	
			std::shared_ptr<ControlStruct> ctrl = mCtrlStruct;

			Condition ready;
			{
				ReadLocker lock(ctrl->rwLock);

				if ( ctrl->messages.tryPop(out) )
					return true;
				else if ( ctrl->endCalled )
					return false;
				else
					ctrl->waiting.push(&ready);
			}

			ready.wait();
			return ctrl->messages.tryPop(out);
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
			WriteLocker lock(mCtrlStruct->rwLock);
			if ( !mCtrlStruct->endCalled )
			{
				mCtrlStruct->endCalled = true;

				Condition* cond = nullptr;
				while ( mCtrlStruct->waiting.tryPop(cond) )
					cond->trigger();
			}
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_PRODUCER_H_
