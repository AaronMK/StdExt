#ifndef _STD_EXT_SIGNALS_EVENT_H_
#define _STD_EXT_SIGNALS_EVENT_H_

#include "../StdExt.h"
#include "../Collections/Vector.h"

#include <cassert>

namespace StdExt::Signals
{
	template<typename ...args_t>
	class EventHandler;

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

		/**
		 * @brief
		 *  Passes the event to all binded handlers.  Overrides should call
		 *  the base implementation.
		 */
		virtual void invoke(const args_t& ...args);

	private:
		void doMove(Event&& other);
		void pruneHandlers();


		Collections::Vector<handler_t*, 2> mHandlers;
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

		virtual void bind(event_t& evt);
		virtual void unbind();

		bool isBinded() const;

	protected:
		virtual void handleEvent(const args_t& ...args) = 0;

	private:
		void doMove(EventHandler&& other);

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
	{
		doMove(std::move(other));
	}

	template<typename ...args_t>
	Event<args_t...>::~Event()
	{
		assert(0 == mActivations);

		for (size_t i = 0; i < mHandlers.size(); ++i)
		{
			handler_t* handler = mHandlers[i];

			if (handler)
				handler->unbind();
		}
	}

	template<typename ...args_t>
	void Event<args_t...>::invoke(const args_t& ...args)
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
		doMove();
		return *this;
	}

	template<typename ...args_t>
	void Event<args_t...>::doMove(Event&& other)
	{
		assert(0 == mActivations);

		mHanders = std::move(other.mHandlers);

		mPrune = other.mPrune;
		other.mPrune = false;

		for (size_t i = 0; i < mHandlers.size(); ++i)
		{
			handler_t* handler = mHandlers[i];

			if (handler && handler->mEvent == &other)
				handler->mEvent = this;
		}
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
	EventHandler<args_t...>::EventHandler() noexcept
		: mEvent(nullptr)
	{
	}

	template<typename ...args_t>
	EventHandler<args_t...>::EventHandler(EventHandler<args_t...>&& other)
		: EventHandler()
	{
		doMove(std::move(other));
	}

	template<typename ...args_t>
	EventHandler<args_t...>::~EventHandler()
	{
		unbind();
	}

	template<typename ...args_t>
	EventHandler<args_t...>& EventHandler<args_t...>::operator=(EventHandler<args_t...>&& other)
	{
		doMove();
		return *this;
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::bind(event_t& evt)
	{
		if (mEvent)
			throw invalid_operation("Can't bind a handler that is already binded.");

		mEvent = &evt;
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
	inline bool EventHandler<args_t...>::isBinded() const
	{
		return (nullptr != mEvent);
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::doMove(EventHandler&& other)
	{
		mEvent = other.mEvent;
		other.mEvent = nullptr;

		if (mEvent)
		{
			size_t index = mEvent->mHandlers.find(&other);
			mEvent->mHandlers[index] = this;
		}
	}
}

#endif // !_STD_EXT_SIGNALS_EVENT_H_