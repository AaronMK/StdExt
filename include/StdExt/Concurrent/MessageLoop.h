#ifndef _STD_EXT_CONCURRENT_MESSAGE_LOOP_H_
#define _STD_EXT_CONCURRENT_MESSAGE_LOOP_H_

#include "../StdExt.h"

#include "../Type.h"
#include "../Concepts.h"
#include "../Exceptions.h"

#include "PredicatedCondition.h"
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
		Scaler<T>;

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
		using msg_container_t = std::variant<std::monostate, T, std::atomic_flag*>;

		Queue<msg_container_t> mMsgQueue;
		PredicatedCondition    mMsgsAvailable;

	public:
		using handler_param_t = std::conditional_t<
			Scaler<T>, T, typename Type<T>::move_t
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
		template<typename... args_t>
		void push(const T& item)
			requires CopyConstructable<T> && Class<T>
		{
			mMsgsAvailable.trigger(
				[&]()
				{
					mMsgQueue.push(item);
				}
			);
		}

		/**
		 * @brief
		 *  Pushes a message into the queue for processing
		 *  using move construction.
		 */
		void push(T&& item)
			requires MoveConstructable<T> && Class<T>
		{
			mMsgsAvailable.trigger(
				[&]()
				{
					mMsgQueue.push( std::move(item) );
				}
			);
		}

		void push(const T& item)
			requires CopyConstructable<T> && Class<T>
		{
			mMsgsAvailable.trigger(
				[&]()
				{
					mMsgQueue.push(item);
				}
			);
		}

		void push(T item)
			requires Scaler<T>
		{
			mMsgsAvailable.trigger(
				[&]()
				{
					mMsgQueue.push(item);
				}
			);
		}

		/**
		 * @brief
		 *  Places a flag in the queue and blocks until that flag is reached.
		 *  This is a way to determine when all items up until this call
		 *  have been processed.
		 */
		void barrier()
		{
			std::atomic_flag condition;
			condition.clear();

			mMsgsAvailable.trigger(
				[&]()
				{
					mMsgQueue.push(&condition);
				}
			);

			condition.wait(false);
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
			mMsgsAvailable.trigger(
				[&]()
				{
					mMsgQueue.push(std::monostate());
				}
			);
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
				try
				{
					mMsgsAvailable.wait(
						[&]()
						{
							return !mMsgQueue.isEmpty();
						}
					);
				}
				catch(const object_destroyed&)
				{
					return;
				}

				msg_container_t msg;
				while (mMsgQueue.tryPop(msg))
				{
					if (std::holds_alternative<T>(msg))
					{
						if constexpr ( Scaler<T> )
							handleMessage(std::get<T>(msg));
						else
							handleMessage( std::move(std::get<T>(msg)) );
					}
					else if (std::holds_alternative<std::atomic_flag*>(msg))
					{
						std::get<std::atomic_flag*>(msg)->test_and_set();
					}
					else
					{
						mMsgsAvailable.destroy();
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
	template<MessageLoopType T, CallableWith<void, typename MessageLoop<T>::handler_param_t> handler_t>
	class FunctionHandlerLoop : public MessageLoop<T>
	{
	public:
		handler_t mHandler;
		using handler_param_t = typename MessageLoop<T>::handler_param_t;

		FunctionHandlerLoop(const handler_t& func)
			: mHandler(func)
		{
		}

		FunctionHandlerLoop(handler_t&& func)
			: mHandler(std::move(func))
		{
		}

	protected:
		virtual void handleMessage(handler_param_t message) override final
		{
			mHandler( std::forward<handler_param_t>(message) );
		}
	};

	template<typename T, CallableWith<void, typename MessageLoop<T>::handler_param_t> handler_t>
	auto makeMessageLoop(const handler_t& func)
	{
		return FunctionHandlerLoop<T, handler_t>(func);
	}

	template<typename T, CallableWith<void, typename MessageLoop<T>::handler_param_t> handler_t>
	auto makeMessageLoop(handler_t&& func)
	{
		return FunctionHandlerLoop<T, handler_t>( std::move(func) );
	}
}

#endif // !_STD_EXT_CONCURRENT_MESSAGE_LOOP_H_