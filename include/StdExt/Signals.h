#ifndef _STD_EXT_SIGNALS_H_
#define _STD_EXT_SIGNALS_H_

#include "StdExt.h"
#include "Memory.h"
#include "Any.h"

#include <type_traits>
#include <functional>
#include <algorithm>
#include <memory>
#include <vector>

namespace StdExt::Signals
{
	template<typename ...args_t>
	class Event;

	template<typename ...args_t>
	class EventHandler;

	template<typename T>
	class Subscription;

	/**
	 * @internal
	 *
	 * @brief
	 *  Classes that used internally and shared by the classes in the Signals namespace.
	 *  They are public for ease of coding and maintenance, but should not be used directly
	 *  by clients of the %Signals namespace.
	 */
	namespace Internal
	{
		/**
		 * @internal
		 *
		 * @brief
		 *  A structure that has shared ownership by Events and EventHandlers.
		 */
		template<typename ...args_t>
		class EvtShared
		{
		public:
			using handler_t = EventHandler<args_t...>;

			std::vector<handler_t*> mHandlers;
			uint32_t activations = 0;
			bool prune = false;
		};
	}

	/**
	 * @brief
	 *  An object that can be invoked to alert subscribed handlers when something
	 *  of interest occurs.
	 */
	template<typename ...args_t>
	class Event
	{
		friend class EventHandler<args_t...>;
	public:

		using handler_t = EventHandler<args_t...>;
		using shared_t = Internal::EvtShared<args_t...>;

		Event();
		virtual ~Event();

		/**
		 * @brief
		 *  Passes the event to all binded handlers.  Overrides should call
		 *  the base implementation.
		 */
		virtual void invoke(args_t ...args);

		/**
		 * @brief
		 *  True if there are any handlers are binded to the event.
		 */
		operator bool() const;

	private:
		std::shared_ptr<shared_t> mShared;
	};

	template<typename ...args_t>
	class EventHandler
	{
		friend class Event<args_t...>;

	public:
		using event_t = Event<args_t...>;
		using evt_shared_t = Internal::EvtShared<args_t...>;

		EventHandler(const EventHandler&) = delete;
		EventHandler& operator=(const EventHandler&) = delete;

		EventHandler();
		EventHandler(EventHandler&& other);

		virtual ~EventHandler();

		EventHandler& operator=(EventHandler&& other);

		void bind(event_t& evt);
		void unbind();

		bool isBinded() const;

	protected:
		std::shared_ptr<typename evt_shared_t> mShared;

		virtual void handleEvent(args_t ...args) = 0;
	};

	template<typename  ...args_t>
	class FunctionHandler : public EventHandler<args_t...>
	{
	public:
		using func_t = std::function<void(args_t...)>;

		FunctionHandler();
		FunctionHandler(func_t&& func);
		FunctionHandler(const func_t& func);

		void setFunction(func_t&& func);
		void setFunction(const func_t& func);

		void clearFunction();

	protected:
		virtual void handleEvent(args_t ...args);

	private:
		func_t mFunc;
	};

	template<typename T>
	class Watchable
	{
		friend class Subscription<T>;
	private:
		Event<T> mEvent;

	protected:

		/**
		 * @brief
		 *  Announces an update to all subscribers.
		 */
		void announceUpdate(const T& val);

	public:
		virtual T value() const = 0;
	};

	template<typename T>
	class Subscription
	{
	public:
		using watch_ptr_t = std::shared_ptr<Watchable<T>>;
		
		Subscription(const watch_ptr_t& watchable);

		/**
		* @brief
		*  Handler function for updates to the watched value.  The default
		*  implementation will do nothing.
		*/
		virtual void onUpdated(const T& val);

		T value() const;

	private:
		watch_ptr_t mWatchable;
		FunctionHandler<T> mHandler;
	};

	template<typename T>
	class Setter : Watchable<T>
	{
	public:
		virtual void setValue(const T& val) = 0;
	};

	class Mapper
	{
	private:
		std::vector<Any> mObjects;

	public:
		template<typename ...args_t>
		void map(Event<args_t...>& evt, std::function<void(args_t...)>&& func)
		{
			using func_t = std::function<void(...args_t)>;
			using handler_t = FunctionHandler<...args_t>;

			Any funcHandler = StdExt::make_any<handler_t>(std::move(func));

			handler_t* funcHandlerPtr = funcHandler.cast<handler_t>();
			funcHandlerPtr->bind(evt);

			mObjects.push_back(std::move(funcHandler));
		}
	};

	////////////////////////////////////////

	namespace Internal
	{

	}

	template<typename ...args_t>
	Event<args_t...>::Event()
	{
		mShared = std::make_shared<Internal::EvtShared<args_t...>>();
	}

	template<typename ...args_t>
	Event<args_t...>::~Event()
	{
	}

	template<typename ...args_t>
	void Event<args_t...>::invoke(args_t ...args)
	{
		++mShared->activations;

		// Use the starting size so that any handlers
		// added won't get called this round.
		size_t origSize = mShared->mHandlers.size();
		for (size_t i = 0; i < origSize; ++i)
		{
			handler_t* currHandler = mShared->mHandlers[i];

			if (nullptr != currHandler)
				currHandler->handleEvent(args...);
		}

		if (0 == --mShared->activations && mShared->prune)
		{
			handler_t** handlerArray = &mShared->mHandlers[0];

			int64_t firstNullIndex = 0;
			int64_t lastNonNullIndex = mShared->mHandlers.size() - 1;

			auto moveToLastNonNull = [&]()
			{
				while(lastNonNullIndex >= 0 && nullptr == handlerArray[lastNonNullIndex])
					--lastNonNullIndex;
			};

			auto moveToNextNull = [&]()
			{
				while(firstNullIndex < lastNonNullIndex && nullptr != handlerArray[firstNullIndex])
					++firstNullIndex;
			};

			moveToLastNonNull();
			moveToNextNull();

			while (lastNonNullIndex > firstNullIndex)
			{
				handlerArray[firstNullIndex] = handlerArray[lastNonNullIndex];
				handlerArray[lastNonNullIndex] = nullptr;

				moveToLastNonNull();
				moveToNextNull();
			}

			mShared->mHandlers.resize(lastNonNullIndex + 1);
			mShared->prune = false;
		}
	}

	template<typename ...args_t>
	Event<args_t...>::operator bool() const
	{
		return (mShared && mShared->mHandlers.size() > 0);
	}

	////////////////////////////////////////

	template<typename ...args_t>
	EventHandler<args_t...>::EventHandler()
	{
	}

	template<typename ...args_t>
	inline EventHandler<args_t...>::EventHandler(EventHandler&& other)
	{
		mShared = std::move(other.mShared);

		if (mShared)
		{
			auto first = mShared->mHandlers.begin();
			auto end = mShared->mHandlers.end();

			auto itr = std::find(first, end, &other);

			if (itr != end)
				(*itr) = this;
		}
	}

	template<typename ...args_t>
	EventHandler<args_t...>::~EventHandler()
	{
		unbind();
	}

	template<typename ...args_t>
	EventHandler<args_t...>& EventHandler<args_t...>::operator=(EventHandler&& other)
	{
		unbind();

		mShared = std::move(other.mShared);

		if (mShared)
		{
			auto first = mShared->mHandlers.begin();
			auto end = mShared->mHandlers.end();

			auto itr = std::find(first, end, &other);

			if (itr != end)
				(*itr) = this;
		}

		return *this;
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::bind(event_t& evt)
	{
		if (mShared)
			throw invalid_operation("Can't bind a handler that is already binded.");

		mShared = evt.mShared;
		evt.mShared->mHandlers.push_back(this);
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::unbind()
	{
		if (mShared)
		{
			auto first = mShared->mHandlers.begin();
			auto end = mShared->mHandlers.end();

			auto itr = std::find(first, end, this);

			if (itr != end)
			{
				if (mShared->activations > 0)
				{
					(*itr) = nullptr;
					mShared->prune = true;
				}
				else
				{
					mShared->mHandlers.erase(itr);
				}
			}

			mShared.reset();
		}
	}

	template<typename ...args_t>
	bool EventHandler<args_t...>::isBinded() const
	{
		return (mShared.get() != nullptr);
	}

	////////////////////////////////////

	template<typename ...args_t>
	FunctionHandler<args_t...>::FunctionHandler()
	{
	}

	template<typename ...args_t>
	FunctionHandler<args_t...>::FunctionHandler(func_t&& func)
	{
		mFunc = std::move(func);
	}

	template<typename ...args_t>
	FunctionHandler<args_t...>::FunctionHandler(const func_t& func)
	{
		mFunc = func;
	}

	template<typename ...args_t>
	void FunctionHandler<args_t...>::setFunction(func_t&& func)
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionHandler<args_t...>::setFunction(const func_t& func)
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = std::move(func);
	}

	template<typename ...args_t>
	void FunctionHandler<args_t...>::clearFunction()
	{
		if (isBinded())
			throw invalid_operation("Can't set the function on a handler that is already binded.");

		mFunc = func_t();
	}

	template<typename ...args_t>
	void FunctionHandler<args_t...>::handleEvent(args_t ...arg)
	{
		if (mFunc)
			mFunc(arg...);
	}

	////////////////////////////////////

	template<typename T>
	void Watchable<T>::announceUpdate(const T& val)
	{
		mEvent.invoke(val);
	}

	////////////////////////////////////

	template<typename T>
	Subscription<T>::Subscription(const watch_ptr_t& watchable)
		: mWatchable(watchable)
	{
		mHandler.setFunction(
			[this](const T& val)
			{
				onUpdated(val);
			}
		);
		mHandler.bind(mWatchable->mEvent);
	}

	template<typename T>
	void Subscription<T>::onUpdated(const T& val)
	{
	}
}

#endif // _STD_EXT_SIGNALS_H_