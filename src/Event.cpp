#include <StdExt/Event.h>

namespace StdExt
{
	Event<void>::Event(void)
	{
		mShared = std::make_shared<EventShared<void>>();
	}

	void Event<void>::invoke() const
	{
		std::unique_lock<std::mutex> listLock(mShared->mListLock);
		size_t handlersSize = mShared->mHandlers.size();

		StackArray<EventHandler<void>*, 16> handlers(handlersSize);

		size_t index = 0;
		for(auto currHandler : mShared->mHandlers)
			handlers[index++] = currHandler;

		listLock.unlock();

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

	EventHandler<void>::EventHandler()
	{
	}

	EventHandler<void>::EventHandler(EventHandler<void>&& other)
	{
		moveFrom(std::move(other));
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

	EventHandler<void>& EventHandler<void>::operator=(EventHandler<void>&& other)
	{
		unbind();
		moveFrom(std::move(other));
		return *this;
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

	void EventHandler<void>::moveFrom(EventHandler<void>&& other)
	{
		if (other.mShared)
		{
			std::scoped_lock<std::mutex> lock(other.mShared->mListLock);

			mShared = other.mShared;
			mHandler = std::move(other.mHandler);

			mShared->mHandlers.erase(&other);
			mShared->mHandlers.insert(this);
		}
	}
}