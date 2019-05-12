#ifndef _STD_EXT_SIGNALS_EVENT_H_
#define _STD_EXT_SIGNALS_EVENT_H_

#include "../StdExt.h"
#include "../Collections/Vector.h"

#include <memory>
#include <vector>

namespace StdExt::Signals
{
	template<typename ...args_t>
	class EventHandler;

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

			Collections::Vector<handler_t*, 2> mHandlers;
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
		virtual void invoke(const args_t& ...args);

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

		virtual void handleEvent(const args_t& ...args) = 0;
	};

	////////////////////////////////////////

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
	void Event<args_t...>::invoke(const args_t& ...args)
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
			try
			{
				size_t index = mShared->mHandlers.find(&other);
				mShared->mHandlers[index] = this;
			}
			catch (const std::exception&)
			{
			}
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
			try
			{
				size_t index = mShared->mHandlers.find(&other);
				mShared->mHandlers[index] = this;
			}
			catch (const std::exception&)
			{
			}
		}

		return *this;
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::bind(event_t& evt)
	{
		if (mShared)
			throw invalid_operation("Can't bind a handler that is already binded.");

		mShared = evt.mShared;
		evt.mShared->mHandlers.emplace_back(this);
	}

	template<typename ...args_t>
	void EventHandler<args_t...>::unbind()
	{
		if (mShared)
		{
			try
			{
				size_t item_index = mShared->mHandlers.find(this);
				
				if (mShared->activations > 0)
				{
					mShared->mHandlers[item_index] = nullptr;
					mShared->prune = true;
				}
				else
				{
					mShared->mHandlers.erase_at(item_index);
				}
			}
			catch (const std::exception&)
			{
			}

			mShared.reset();
		}
	}

	template<typename ...args_t>
	bool EventHandler<args_t...>::isBinded() const
	{
		return (mShared.get() != nullptr);
	}
}

#endif // !_STD_EXT_SIGNALS_EVENT_H_