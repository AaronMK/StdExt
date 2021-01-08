#ifndef _STD_EXT_SIGNALS_AGGREGATOR_H_
#define _STD_EXT_SIGNALS_AGGREGATOR_H_

#include "Subscription.h"

#include <utility>
#include <functional>

namespace StdExt::Signals
{
	/**
	 * @brief
	 *  Watches a range of subscriptions and makes it easy to apply custom shared logic on them.
	 */
	template<typename in_t, typename out_t = in_t>
	class Aggregator : public Watchable<out_t>
	{
	private:
		class IndexPusher : public Subscription<in_t>
		{
			using pass_t = Subscription<in_t>::pass_t;
		public:
			size_t mIndex;
			Aggregator* mParent;

			IndexPusher(const Watchable<in_t>& _watchable, size_t _index, Aggregator* _parent)
			{
				mIndex = _index;
				mParent = _parent;
				Subscription<in_t>::attach(_watchable);
			}

		protected:
			virtual void onUpdated(pass_t val) override
			{
				mParent->onUpdate(mIndex, val);
			}
		};

		Collections::Vector<IndexPusher, 2, 1> mSubscriptions;

	protected:
		WatchablePassType<in_t> valueAt(size_t index) const
		{
			return mSubscriptions[index].value();
		}

		size_t size() const
		{
			return mSubscriptions.size();
		}

		void forEachSub(const std::function<void(WatchablePassType<in_t>)>& func)
		{
			for(size_t i = 0; i < mSubscriptions.size(); ++i)
				func(mSubscriptions[i].value());
		}

		/**
		 * @brief
		 *  Called when any of the aggregated values is updated.  Implmentation should
		 *  calculate the aggregate and call updateValue() on the result.
		 */
		virtual void onUpdate(size_t index, WatchablePassType<in_t> value) = 0;

		/**
		 * @brief
		 *  Initializes all subscriptions for the aggregator.  Subclasses should call
		 *  this constructor, and use updateValue() to set an initial value.
		 */
		Aggregator(std::initializer_list<const Watchable<in_t>&> args)
		{
			mSubscriptions.reserve(args.size());

			size_t index = 0;
			std::for_each(args.begin(), args.end(),
				[&](const Watchable<in_t>& arg)
				{
					mSubscriptions.emplace_back(arg, index, this);
					++index;
				}
			);
		}

	public:
		Aggregator(const Aggregator& other) = delete;
		Aggregator& operator=(const Aggregator& other) = delete;

		Aggregator(Aggregator&& other)
			: Watchable<out_t>(std::move(other)),
			  mSubscriptions(std::move(other.mSubscriptions))
		{
			for (size_t i = 0; i < mSubscriptions.size(); ++i)
				mSubscriptions[i].mParent = this;
		}

		Aggregator& operator=(Aggregator&& other)
		{
			Aggregator(std::move(other));
			return *this;
		}
	};
}

#endif // !_STD_EXT_SIGNALS_AGGREGATOR_H_