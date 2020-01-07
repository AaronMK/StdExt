#ifndef _STD_EXT_SIGNALS_EVENT_H_
#define _STD_EXT_SIGNALS_EVENT_H_

#include "../StdExt.h"
#include "../String.h"

#include "../Collections/Vector.h"

#include <cassert>

namespace StdExt::Signals
{
	template<typename ...args_t>
	class EventHandler;

	/**
	 * @brief
	 *  An event is the base for an object that can raise events that will be passed any number
	 *  of subscribed handlers.  It serves is the bases for signaling and response mechanisms.
	 *  In addition to message types of its template parameters, it can also serve to enable
	 *  tracking of the movement and destruction of objects.
	 */
	template<typename ...args_t>
	class Event
	{
	public:
		using handler_t = EventHandler<args_t...>;

		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;

		Event() noexcept;
		Event(Event&& other) noexcept;

		virtual ~Event();

		Event& operator=(Event&& other);

		String Name;

	protected:

		/**
		 * @brief
		 *  Passes a notification to all handlers to respond to a change in the object.
		 *  Overrides should call the base function at the end.
		 */
		virtual void notify(const args_t& ...args);

	private:
		void pruneHandlers();

		/**
		 * @brief
		 *  A list of all subscribed event handlers.
		 */
		Collections::Vector<handler_t*, 2, 8> mHandlers;
		
		/**
		 * @brief
		 *  This keeps track of the invokations of the active event.
		 */
		uint32_t mActivations = 0;

		/**
		 * @brief
		 *  This is a flag that can be set to tell the event
		 *  that there are nullptr entries in mHandlers and
		 *  that mHandlers can be pruned.
		 */
		bool mPrune = false;

		friend class handler_t;
	};

	/**
	 * @brief
	 *  Base class for events that can be externally invoked.
	 */
	template <typename ...args_t>
	class Invokable : public virtual Event<args_t...>
	{
	public:

		/**
		 * @brief
		 *  The default implementation simply calls the internally protected notify().
		 */
		virtual void invoke(const args_t& ...args);
	};

	template<typename ...args_t>
	class EventHandler
	{
	public:
		using event_t = Event<args_t...>;

		EventHandler(const EventHandler&) = delete;
		EventHandler& operator=(const EventHandler&) = delete;

		EventHandler() noexcept;
		EventHandler(EventHandler&& other);

		virtual ~EventHandler();

		EventHandler& operator=(EventHandler&& other);

		virtual void bind(const event_t& evt);
		virtual void unbind();

		/**
		 * @brief
		 *  Returns true if the handler is binded to an event.
		 */
		bool isBinded() const;

		/**
		 * @brief
		 *  Gets a pointer to the source object generating events if binded.
		 */
		const event_t* source() const;
		event_t* source();

	protected:

		/**
		 * @brief
		 *  Called to handle invoked events.  The default implementation does nothing.
		 */
		virtual void handleEvent(const args_t& ...args);

		/**
		 * @brief
		 *  A handler that is called when a binded event is destroyed.  The default implementation puts
		 *  this handler in an unbinded state, and should be first called in overrides.
		 */
		virtual void onSourceDestroyed();

		/**
		 * @brief
		 *  Called when the attached event has moved.  This can be overriden to add additional actions in
		 *  reponse to the moving of an object.  The default implementation updates the internal
		 *  pointer used for binding and should be called first in overrides.
		 */
		virtual void onSourceMoved(event_t* nextAddress);


	private:

		/**
		 * @brief
		 *  A pointer to the binded event, or nullptr if there is no event attached.
		 */
		event_t* mEvent;

		friend class event_t;
	};

	////// Implementation //////

	template<typename ...args_t>
	Event<args_t...>::Event() noexcept
	{
	}

	template<typename ...args_t>
	Event<args_t...>::Event(Event&& other) noexcept
		: Event()
	{
		(*this) = std::move(other);
	}

	template<typename ...args_t>
	Event<args_t...>::~Event()
	{
		assert(0 == mActivations);

		for (size_t i = 0; i < mHandlers.size(); ++i)
		{
			handler_t* handler = mHandlers[i];

			if (handler)
			{
				handler->mEvent = nullptr;
				handler->onSourceDestroyed();
			}
		}
	}

	template<typename ...args_t>
	void Event<args_t...>::notify(const args_t& ...args)
	{
		++mActivations;

		// Use the starting size so that any handlers
		// added won't get called this round.
		size_t origSize = mHandlers.size();
		for (size_t i = 0; i < origSize; ++i)
		{
			handler_t* currHandler = mHandlers[i];

			if (nullptr != currHandler)
				currHandler->handleEvent(args...);
		}

		if (0 == --mActivations && mPrune)
			pruneHandlers();
	}

	template<typename ...args_t>
	Event<args_t...>& Event<args_t...>::operator=(Event<args_t...>&& other)
	{
		assert(0 == other.mActivations);

		mHanders = std::move(other.mHandlers);

		mPrune = other.mPrune;
		other.mPrune = false;

		for (size_t i = 0; i < mHandlers.size(); ++i)
		{
			handler_t* handler = mHandlers[i];

			if (handler && handler->mEvent == &other)
			{
				handler->mEvent = this;
				handler->onSourceMoved(this);
			}
		}

		return *this;
	}

	template<typename ...args_t>
	void Event<args_t...>::pruneHandlers()
	{
		assert(0 == mActivations);

		if (mPrune)
		{
			handler_t** handlerArray = &mHandlers[0];

			int64_t firstNullIndex = 0;
			int64_t lastNonNullIndex = mHandlers.size() - 1;

			// Adjusts lastNonNullIndex to be the index of the last entry that is non-null.
			auto moveToLastNonNull = [&]()
			{
				while (lastNonNullIndex >= 0 && nullptr == handlerArray[lastNonNullIndex])
					--lastNonNullIndex;
			};

			// Adjusts firstNullIndex to be the index of the first entry that is null.
			auto moveToNextNull = [&]()
			{
				while (firstNullIndex < lastNonNullIndex && nullptr != handlerArray[firstNullIndex])
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

			mHandlers.resize(lastNonNullIndex + 1);
			mPrune = false;
		}
	}

	///////////////////////

	template<typename ...args_t>
	void Invokable<args_t...>::invoke(const args_t& ...args)
	{
		notify(std::forward<args_t>(args)...);
	}

	///////////////////////

	template<typename ...args_t>
	EventHandler<args_t...>::EventHandler() noexcept
		: mEvent(nullptr)
	{
	}

	template<typename ...args_t>
	EventHandler<args_t...>::EventHandler(EventHandler<args_t...>&& other)
		: EventHandler()
	{
		(*this) = std::move(other);
	}

	template<typename ...args_t>
	EventHandler<args_t...>::~EventHandler()
	{
		unbind();
	}

	template<typename ...args_t>
	EventHandler<args_t...>& EventHandler<args_t...>::operator=(EventHandler<args_t...>&& other)
	{
		mEvent = movePtr(other.mEvent);

		if (mEvent)
		{
			size_t index = mEvent->mHandlers.find(&other);
			mEvent->mHandlers[index] = this;
		}

		return *this;
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::bind(const event_t& evt)
	{
		unbind();

		mEvent = const_cast<event_t*>(&evt);
		mEvent->mHandlers.emplace_back(this);
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::unbind()
	{
		if (mEvent)
		{
			size_t index = mEvent->mHandlers.find(this);
			mEvent->mHandlers[index] = nullptr;
			mEvent->mPrune = true;

			mEvent = nullptr;
		}
	}

	template<typename ...args_t>
	bool EventHandler<args_t...>::isBinded() const
	{
		return (nullptr != mEvent);
	}

	template<typename ...args_t>
	const Event<args_t...>* EventHandler<args_t...>::source() const
	{
		return mEvent;
	}

	template<typename ...args_t>
	Event<args_t...>* EventHandler<args_t...>::source()
	{
		return mEvent;
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::handleEvent(const args_t& ...args)
	{
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::onSourceDestroyed()
	{
		mEvent = nullptr;
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::onSourceMoved(event_t* newAddress)
	{
	}
}

#endif // !_STD_EXT_SIGNALS_EVENT_H_