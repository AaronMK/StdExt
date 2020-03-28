#ifndef _STD_EXT_SIGNALS_AGGREGATOR_H_
#define _STD_EXT_SIGNALS_AGGREGATOR_H_

#include "Event.h"

#include <utility>

namespace StdExt::Signals
{
	/**
	 * @brief
	 *  Watches a range of events and makes it easy to apply custom shared logic on them.
	 */
	template<typename ...args_t>
	class Aggregator
	{
		class IndexPusher : public EventHandler<args_t...>
		{
		public:
			size_t index;
			Aggregator* parent;

			IndexPusher(const Event<args_t...>& _evt, size_t _index, Aggregator* _parent)
			{
				index = _index;
				parent = _parent;
				bind(_evt);
			}

			virtual void handleEvent(args_t ...args) override
			{
				parent->onNotify(index, std::forward<args_t>(args)...);
			}
		};

		Collections::Vector<IndexPusher, 2, 4> mSubscriptions;

	public:
		Aggregator()
		{

		}

		Aggregator(std::initializer_list<const Event<args_t...>&> args)
		{
			setInputs(args);
		}

		void setInputs(std::initializer_list<const Event<args_t...>&> args)
		{
			mHandlers.resize(0);
			mHandlers.reserve(args.size());

			size_t index = 0;
			for_each(args.begin(), args.end(),
				[&](const Watchable<T>& arg)
				{
					mSubscriptions.emplace_back(arg, index, this);
					++index;
				}
			);
		}

		Aggregator(Aggregator&& other)
			: Event(std::move(other)),
			  mSubscriptions(std::move(other.mSubscriptions))
		{
			for (size_t i = 0; i < mSubscriptions.size(); ++i)
				mSubscriptions[i].parent = this;
		}

		Aggregator& operator=(Aggregator&& other)
		{
			Aggregator(std::move(other));
			return *this;
		}

		const Event<args_t...>& operator[](size_t index) const
		{
			return *mSubscriptions[index].source();
		}

		Event<args_t...>& operator[](size_t index)
		{
			return *mSubscriptions[index].source();
		}

		size_t size() const
		{
			return mSubscriptions.size();
		}

		/**
		 * @brief
		 *  Called with the index of the event and the parameters of the notification.
		 */
		virtual void onNotify(size_t index, args_t ...args) {};
	};
}

#endif // !_STD_EXT_SIGNALS_AGGREGATOR_H_