#ifndef _STD_EXT_EVENT_H_
#define _STD_EXT_EVENT_H_

#include "UID.h"
#include "Memory.h"

#include <functional>
#include <memory>
#include <mutex>
#include <set>

namespace StdExt
{
	template<typename ...Args>
	class Event;

	template<typename ...Args>
	class EventHandler;

	template<typename ...Args>
	class EventShared
	{
		friend class Event<Args...>;
		friend class EventHandler<Args...>;

	private:
		std::mutex mListLock;
		std::set<EventHandler<Args...>*> mHandlers;
	};

	template<>
	class EventShared<void>
	{
		friend class Event<void>;
		friend class EventHandler<void>;

	private:
		std::mutex mListLock;
		std::set<EventHandler<void>*> mHandlers;
	};

	template<typename ...Args>
	class EventHandler final
	{
		friend class Event<...Args>;

	public:
		using func_t = std::function<void(Args...)>;
		using event_t = Event<Args...>;

		EventHandler();

		EventHandler(const EventHandler&) = delete;

		EventHandler(const event_t& evt, func_t&& handler);
		EventHandler(const event_t& evt, const func_t& handler);

		~EventHandler();

		void bind(const event_t& evt, func_t&& handler);
		void bind(const event_t& evt, const func_t& handler);

		void unbind();

	private:
		func_t mHandler;
		std::shared_ptr<EventShared<Args...>> mShared;
	};

	template<>
	class EventHandler<void> final
	{
		friend class Event<void>;

	public:
		using func_t = std::function<void()>;
		using event_t = Event<void>;

		EventHandler();

		EventHandler(const EventHandler&) = delete;

		EventHandler(const event_t& evt, func_t&& handler);
		EventHandler(const event_t& evt, const func_t& handler);

		~EventHandler();

		void bind(const event_t& evt, func_t&& handler);
		void bind(const event_t& evt, const func_t& handler);

		void unbind();

	private:
		func_t mHandler;
		std::shared_ptr<EventShared<void>> mShared;
	};

	template<typename... Args>
	class Event
	{
		friend class EventHandler<Args...>;

	private:
		mutable std::shared_ptr<EventShared<Args...>> mShared;

	public:
		Event();

		void invoke(Args&& ...arguments) const;
		operator bool() const;
	};

	template<>
	class Event<void>
	{
		friend class EventHandler<void>;

	private:
		mutable std::shared_ptr<EventShared<void>> mShared;

	public:
		Event();

		void invoke() const;
		operator bool() const;
	};

	/////////////////////////////////////////

	template<typename... Args>
	Event<Args...>::Event()
	{
		mShared = std::make_shared<EventShared<Args...>>();
	}

	template<typename... Args>
	void Event<Args...>::invoke(Args&& ...arguments) const
	{
		mShared->mListLock.lock();
		size_t handlersSize = mShared->mHandlers.size();

		StackArray<EventHandler<Args...>*, 16> handlers(handlersSize);

		size_t index = 0;
		for(auto currHandler : mShared->mHandlers)
			handlers[index++] = currHandler;

		mShared->mListLock.unlock();

		for(size_t i = 0; i < handlers.size(); ++i)
		{
			if (handlers[i]->mHandler)
				handlers[i]->mHandler(std::forward<Args>(arguments)...);
		}
	}

	template<typename... Args>
	Event<Args...>::operator bool() const
	{
		return (mShared->mHandlers.size() > 0);
	}

	/////////////////////////////////////////

	Event<void>::Event(void)
	{
		mShared = std::make_shared<EventShared<void>>();
	}

	void Event<void>::invoke() const
	{
		mShared->mListLock.lock();
		size_t handlersSize = mShared->mHandlers.size();

		StackArray<EventHandler<void>*, 16> handlers(handlersSize);

		size_t index = 0;
		for(auto currHandler : mShared->mHandlers)
			handlers[index++] = currHandler;

		mShared->mListLock.unlock();

		for(size_t i = 0; i < handlers.size(); ++i)
		{
			if (handlers[i]->mHandler)
				handlers[i]->mHandler();
		}
	}

	Event<void>::operator bool() const
	{
		return (mShared->mHandlers.size() > 0);
	}

	/////////////////////////////////////

	template<typename ...Args>
	EventHandler<Args...>::EventHandler()
	{
	}

	template<typename ...Args>
	EventHandler<Args...>::EventHandler(const event_t& evt, func_t&& handler)
	{
		bind(evt, std::move(handler));
	}

	template<typename ...Args>
	EventHandler<Args...>::EventHandler(const event_t& evt, const func_t& handler)
	{
		bind(evt, handler);
	}
	
	template<typename ...Args>
	EventHandler<Args...>::~EventHandler()
	{
		unbind();
	}

	template<typename ...Args>
	void EventHandler<Args...>::bind(const event_t& evt, func_t&& handler)
	{
		unbind();
		mShared = evt.mShared;

		std::scoped_lock<std::mutex> lock(mShared->mListLock);
		mHandler = std::move(handler);
		mShared->mHandlers.insert(this);
	}

	template<typename ...Args>
	void EventHandler<Args...>::bind(const event_t& evt, const func_t& handler)
	{
		unbind();
		mShared = evt.mShared;

		std::scoped_lock<std::mutex> lock(mShared->mListLock);
		mHandler = handler;
		mShared->mHandlers.insert(this);
	}

	template<typename ...Args>
	void EventHandler<Args...>::unbind()
	{
		if (mShared)
		{
			std::scoped_lock<std::mutex> lock(mShared->mListLock);
			mShared->mHandlers.erase(this);

			mShared.reset();
		}
	}

	/////////////////////////////////////

	EventHandler<void>::EventHandler()
	{
	}

	EventHandler<void>::EventHandler(const event_t& evt, func_t&& handler)
	{
		bind(evt, std::move(handler));
	}

	EventHandler<void>::EventHandler(const event_t& evt, const func_t& handler)
	{
		bind(evt, handler);
	}

	EventHandler<void>::~EventHandler()
	{
		unbind();
	}

	void EventHandler<void>::bind(const event_t& evt, func_t&& handler)
	{
		unbind();
		mShared = evt.mShared;

		std::scoped_lock<std::mutex> lock(mShared->mListLock);
		mHandler = std::move(handler);
		mShared->mHandlers.insert(this);
	}

	void EventHandler<void>::bind(const event_t& evt, const func_t& handler)
	{
		unbind();
		mShared = evt.mShared;

		std::scoped_lock<std::mutex> lock(mShared->mListLock);
		mHandler = handler;
		mShared->mHandlers.insert(this);
	}

	void EventHandler<void>::unbind()
	{
		if (mShared)
		{
			std::scoped_lock<std::mutex> lock(mShared->mListLock);
			mShared->mHandlers.erase(this);

			mShared.reset();
		}
	}
}

#endif // _STD_EXT_EVENT_H_
