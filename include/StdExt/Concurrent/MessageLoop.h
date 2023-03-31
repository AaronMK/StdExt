#ifndef _STD_EXT_CONCURRENT_MESSAGE_LOOP_H_
#define _STD_EXT_CONCURRENT_MESSAGE_LOOP_H_

#include "../StdExt.h"

#include "../Type.h"
#include "../Concepts.h"
#include "../Exceptions.h"

#include "Condition.h"
#include "Queue.h"
#include "Task.h"

#include <cassert>
#include <utility>
#include <variant>
#include <functional>

namespace StdExt::Concurrent
{
	/**
	 * @brief
	 *  Concept defining the types allowed for message loops.  It must be a class/struct
	 *  type with copy and/or move support, or a scaler.
	 */
	template<typename T>
	concept MessageLoopType =
		(Class<T> && (MoveConstructable<T> || CopyConstructable<T>)) ||
		Scaler<T> || ReferenceType<T>;

	/**
	 * @brief
	 *  The Message Loop is a class to process messages in the order in which they
	 *  are received in a seperate context from the sender.  It is a subclass of
	 *  Task, and the runAsync() and runAsThread() members will start processing of
	 *  messages.
	 */
	template<MessageLoopType T>
	class MessageLoop : public Task
	{
	private:
		using storage_t = std::conditional_t<ReferenceType<T>, void*, T>;
		using msg_container_t = std::variant<std::monostate, storage_t, Condition*>;

		Queue<msg_container_t> mMsgQueue;
		Condition mMsgsAvailable;
		std::atomic<bool> mEndCalled = false;

	public:
		using handler_param_t = std::conditional_t<
			Scaler<T> || ReferenceType<T>,
			T, 
			std::conditional_t<
				ConstType<T>,
				typename Type<T>::const_reference_t,
				typename Type<T>::reference_t
			>
		>;

		MessageLoop()
		{
		}

		/**
		 * @brief
		 *  Destruction of the message loop.  It is the responsibility
		 *  of client or subclass code to call end() and wait for the
		 *  task to finish before this destructor is called.
		 */
		virtual ~MessageLoop()
		{
			assert( !isRunning() );
		}

		/**
		 * @brief
		 *  Pushes a message into the queue for processing
		 *  using copy construction.
		 */
		void push(const T& item)
			requires CopyConstructable<T> && Class<T>
		{
			mMsgQueue.push(item);
			mMsgsAvailable.trigger();
		}

		/**
		 * @brief
		 *  Pushes a message into the queue for processing
		 *  using move construction.
		 */
		void push(T&& item)
			requires MoveConstructable<T> && Class<T>
		{
			mMsgQueue.push(std::move(item));
			mMsgsAvailable.trigger();
		}

		void push(T item)
			requires Scaler<T>
		{
			mMsgQueue.push(item);
			mMsgsAvailable.trigger();
		}

		void  push(T item)
			requires ReferenceType<T>
		{
			mMsgQueue.push( access_as<void*>(std::addressof(item)) );
			mMsgsAvailable.trigger();
		}

		/**
		 * @brief
		 *  Places a flag in the queue and blocks until that flag is reached.
		 *  This is a way to determine when all items up until this call
		 *  have been processed.
		 */
		void barrier()
		{
			Condition condition;
			mMsgQueue.push(&condition);

			mMsgsAvailable.trigger();
			condition.wait();
		}

		/**
		 * @brief
		 *  Puts a message at the end of the queue telling processing to stop
		 *  once seen.  Any messages passed after this call will be ignored.
		 *  This will return instantly.  wait() can be used to determine when
		 *  previous messages have finished processing.
		 */
		void end()
		{
			mMsgQueue.push(std::monostate());
			mMsgsAvailable.trigger();
		}

	protected:

		/**
		 * @brief
		 *  Override will be called to process each message.  For class/struct types
		 *  the value will be passed by const reference.  For scaler types, the value
		 *  will be passed by copy.
		 */
		virtual void handleMessage(handler_param_t message) = 0;

		virtual void run() override final
		{
			while (true)
			{
				mMsgsAvailable.wait();
				mMsgsAvailable.reset();

				msg_container_t msg;
				while (mMsgQueue.tryPop(msg))
				{
					if (std::holds_alternative<storage_t>(msg))
					{
						if constexpr ( ReferenceType<T> )
							handleMessage( access_as<T>(std::get<storage_t>(msg)) );
						else if constexpr ( Scaler<T> )
							handleMessage(std::get<storage_t>(msg));
						else
							handleMessage(access_as<handler_param_t>(&std::get<storage_t>(msg)));
							
					}
					else if (std::holds_alternative<Condition*>(msg))
					{
						std::get<Condition*>(msg)->trigger();
					}
					else
					{
						return;
					}
				}
			}
		}
	};

	/**
	 * @brief
	 *  A message loop subclass that takes a function at construction the is used
	 *  to process each message pushed into the loop.
	 */
	template<MessageLoopType T>
	class FunctionHandlerLoop : public MessageLoop<T>
	{
	public:
		using handler_t = std::function<void(typename MessageLoop<T>::handler_param_t)>;

		handler_t mHandler;

		FunctionHandlerLoop(const handler_t& func)
			: mHandler(func)
		{
		}

		FunctionHandlerLoop(handler_t&& func)
			: mHandler(std::move(func))
		{
		}

	protected:
		virtual void handleMessage(typename MessageLoop<T>::handler_param_t message) override final
		{
			mHandler(message);
		}
	};
}

#endif // !_STD_EXT_CONCURRENT_MESSAGE_LOOP_H_