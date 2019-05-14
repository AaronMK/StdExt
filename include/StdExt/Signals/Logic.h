#ifndef _STD_EXT_SIGNALS_LOGIC_H_
#define _STD_EXT_SIGNALS_LOGIC_H_

#include "Watchable.h"
#include "Constant.h"
#include "FunctionHandlers.h"

#include "../Any.h"
#include "../Type.h"
#include "../Utility.h"
#include "../ParameterPack.h"

#include "../Collections/Vector.h"

#include <memory>

namespace StdExt::Signals
{
	class And : public Watchable<bool>
	{
	private:
		bool lastValue;
		Collections::Vector<FunctionUpdateHandler<bool>, 2, 4> mHandlers;
		
		void handler(const bool& val)
		{
			if ( update(lastValue, value()) )
				announceUpdate(lastValue);
		};

	public:
		template<typename ...args_t>
		And(args_t ...arguments)
		{
			static_assert(can_assign_from_v<WatchRef<bool>, args_t...>, "All arguments must be convertable to WatchRef<bool>.");
			std::initializer_list<WatchRef<bool>> args({ std::forward<args_t>(arguments)... });

			mHandlers.resize(args.size());
			const WatchRef<bool>* argPtrs = args.begin();
			lastValue = true;

			for (size_t i = 0; i < args.size(); ++i)
			{
				mHandlers[i].setFunction(std::bind(&And::handler, this, std::placeholders::_1));
				mHandlers[i].attach(argPtrs[i]);
				lastValue = (lastValue && mHandlers[i].value());
			}
		}

		virtual bool value() const override
		{
			for (size_t i = 0; i < mHandlers.size(); ++i)
			{
				if (mHandlers[i].value() == false)
					return false;
			}

			return true;
		}
	};

	class Or : public Watchable<bool>
	{
	private:
		bool lastValue;
		Collections::Vector<FunctionUpdateHandler<bool>, 2, 4> mHandlers;

		void handler(const bool& val)
		{
			if ( update(lastValue, value()) )
				announceUpdate(lastValue);
		};

	public:
		template<typename ...args_t>
		Or(args_t ...arguments)
		{
			static_assert(can_assign_from_v<WatchRef<bool>, args_t...>, "All arguments must be convertable to WatchRef<bool>.");
			std::initializer_list<WatchRef<bool>> args({ std::forward<args_t>(arguments)... });

			mHandlers.resize(args.size());
			WatchRef<bool>* argPtrs;
			lastValue = false;

			for (size_t i = 0; i < args.size(); ++i)
			{
				mHandlers[i].setFunction(std::bind(&Or::handler, this, std::placeholders::_1));
				mHandlers[i].attach(argPtrs[i]);
				lastValue = (lastValue || mHandlers[i].value());
			}
		}

		virtual bool value() const override
		{
			for (size_t i = 0; i < mHandlers.size(); ++i)
			{
				if (mHandlers[i].value())
					return true;
			}

			return false;
		}
	};

	class Count : public Watchable<int>
	{
	private:
		int lastValue;
		Collections::Vector<FunctionUpdateHandler<bool>, 8, 4> mHandlers;

		void handler(const bool& val)
		{
			if ( update( lastValue, value()) )
				announceUpdate(lastValue);
		};

	public:
		template<typename ...args_t>
		Count(args_t ...arguments)
		{
			static_assert(can_assign_from_v<WatchRef<bool>, args_t...>, "All arguments must be convertable to WatchRef<bool>.");
			std::initializer_list<WatchRef<bool>> args({ std::forward<args_t>(arguments)... });

			mHandlers.resize(args.size());
			WatchRef<bool>* argPtrs;
			lastValue = 0;

			for (size_t i = 0; i < args.size(); ++i)
			{
				mHandlers[i].setFunction(std::bind(&Or::handler, this, std::placeholders::_1));
				mHandlers[i].attach(argPtrs[i]);
				lastValue += mHandlers[i].value() ? 1 : 0;
			}
		}

		virtual int value() const override
		{
			int val = 0;

			for (size_t i = 0; i < mHandlers.size(); ++i)
				val += mHandlers[i].value() ? 1 : 0;

			return val;
		}
	};

	class Xor : public Watchable<bool>
	{
	private:

		bool lastValue;
		FunctionUpdateHandler<bool> mLeft;
		FunctionUpdateHandler<bool> mRight;
		
		void handler(const bool& val)
		{
			if ( update(lastValue, mLeft.value() != mRight.value()) )
				announceUpdate(lastValue);
		}

	public:
		Xor(const std::shared_ptr<Watchable<bool>>& left, const std::shared_ptr<Watchable<bool>>& right)
		{
			mLeft.setFunction(std::bind(&Xor::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&Xor::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = (mLeft.value() != mRight.value());
		}

		virtual bool value() const override
		{
			return (mLeft.value() != mRight.value());
		}
	};

	class Nor : public Watchable<bool>
	{
	private:

		bool lastValue;
		FunctionUpdateHandler<bool> mLeft;
		FunctionUpdateHandler<bool> mRight;

		void handler(const bool& val)
		{

			if ( update(lastValue, !mLeft.value() && !mRight.value()) )
				announceUpdate(lastValue);
		}

	public:
		Nor(const std::shared_ptr<Watchable<bool>>& left, const std::shared_ptr<Watchable<bool>>& right)
		{
			mLeft.setFunction(std::bind(&Nor::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&Nor::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = (!mLeft.value() && !mRight.value());
		}

		virtual bool value() const override
		{
			return (!mLeft.value() && !mRight.value());
		}
	};

	class Not : public Watchable<bool>
	{
	private:
		bool lastValue;
		FunctionUpdateHandler<bool> mInput;

		void handler(const bool& val)
		{
			if ( update(lastValue, !val) )
				announceUpdate(lastValue);
		}

	public:
		Not(const std::shared_ptr<Watchable<bool>>& input)
		{
			mInput.setFunction(std::bind(&Not::handler, this, std::placeholders::_1));
			mInput.attach(input);

			lastValue = !mInput.value();
		}

		virtual bool value() const override
		{
			return !mInput.value();
		}
	};

	template<typename T>
	class LessThan : public Watchable<bool>
	{
		bool lastValue;
		FunctionUpdateHandler<T> mLeft;
		FunctionUpdateHandler<T> mRight;

		void handler(const bool& val)
		{
			if ( update(lastValue, mLeft.value() < mRight.value()) )
				announceUpdate(lastValue);
		}

	public:
		static_assert(Traits<T>::has_less_than, "T must support < operator.");

		LessThan(const std::shared_ptr<Watchable<T>>& left, const std::shared_ptr<Watchable<T>>& right)
		{
			mLeft.setFunction(std::bind(&LessThan::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&LessThan::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = (mLeft.value() < mRight.value());
		}

		LessThan(const T& left, const std::shared_ptr<Watchable<T>>& right)
			: LessThan(std::make_shared<ConstWatchable<T>>(left), right)
		{
		}

		LessThan(const std::shared_ptr<Watchable<T>>& left, const T& right)
			: LessThan(left, std::make_shared<ConstWatchable<T>>(right))
		{
		}

		virtual bool value() const override
		{
			return (mLeft.value() < mRight.value());
		}
	};

	template<typename T>
	class LessThanEqual : public Watchable<bool>
	{
		bool lastValue;
		FunctionUpdateHandler<T> mLeft;
		FunctionUpdateHandler<T> mRight;

		void handler(const bool& val)
		{
			if ( update(lastValue, mLeft.value() <= mRight.value()) )
				announceUpdate(lastValue);
		}

	public:
		static_assert(Traits<T>::has_less_equal, "T must support <= operator.");

		LessThanEqual(const std::shared_ptr<Watchable<T>>& left, const std::shared_ptr<Watchable<T>>& right)
		{
			mLeft.setFunction(std::bind(&LessThanEqual::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&LessThanEqual::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = (mLeft.value() <= mRight.value());
		}

		LessThanEqual(const T& left, const std::shared_ptr<Watchable<T>>& right)
			: LessThanEqual(std::make_shared<ConstWatchable<T>>(left), right)
		{
		}

		LessThanEqual(const std::shared_ptr<Watchable<T>>& left, const T& right)
			: LessThanEqual(left, std::make_shared<ConstWatchable<T>>(right))
		{
		}

		virtual bool value() const override
		{
			return (mLeft.value() <= mRight.value());
		}
	};

	template<typename T>
	class Equal : public Watchable<bool>
	{
		bool lastValue;
		FunctionUpdateHandler<T> mLeft;
		FunctionUpdateHandler<T> mRight;

		bool isEqual() const
		{
			if constexpr (std::is_floating_point_v<T>)
				return approximately_equal(mLeft.value(), mRight.value());
			else
				return (mLeft.value() == mRight.value());
		}

		void handler(const bool& val)
		{
			if (update(lastValue, isEqual()))
				announceUpdate(lastValue);
		}

	public:
		static_assert(Traits<T>::has_equality, "T must support == operator.");

		Equal(const std::shared_ptr<Watchable<T>>& left, const std::shared_ptr<Watchable<T>>& right)
		{
			mLeft.setFunction(std::bind(&Equal::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&Equal::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = isEqual();
		}

		Equal(const T & left, const std::shared_ptr<Watchable<T>> & right)
			: Equal(std::make_shared<ConstWatchable<T>>(left), right)
		{
		}

		Equal(const std::shared_ptr<Watchable<T>> & left, const T & right)
			: Equal(left, std::make_shared<ConstWatchable<T>>(right))
		{
		}

		virtual bool value() const override
		{
			return isEqual();
		}
	};

	template<typename T>
	class NotEqual : public Watchable<bool>
	{
		bool lastValue;
		FunctionUpdateHandler<T> mLeft;
		FunctionUpdateHandler<T> mRight;

		bool isNotEqual() const
		{
			if constexpr (std::is_floating_point_v<T>)
				return !approximately_equal(mLeft.value(), mRight.value());
			else
				return (mLeft.value() != mRight.value());
		}

		void handler(const bool& val)
		{
			if ( update(lastValue, isNotEqual()) )
				announceUpdate(lastValue);
		}

	public:
		static_assert(Traits<T>::has_equality, "T must support != operator.");

		NotEqual(const std::shared_ptr<Watchable<T>> & left, const std::shared_ptr<Watchable<T>> & right)
		{
			mLeft.setFunction(std::bind(&NotEqual::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&NotEqual::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = isNotEqual();
		}

		NotEqual(const T & left, const std::shared_ptr<Watchable<T>> & right)
			: NotEqual(std::make_shared<ConstWatchable<T>>(left), right)
		{
		}

		NotEqual(const std::shared_ptr<Watchable<T>> & left, const T & right)
			: NotEqual(left, std::make_shared<ConstWatchable<T>>(right))
		{
		}

		virtual bool value() const override
		{
			return isNotEqual();
		}
	};

	template<typename T>
	class GreaterThanEqual : public Watchable<bool>
	{
		bool lastValue;
		FunctionUpdateHandler<T> mLeft;
		FunctionUpdateHandler<T> mRight;

		void handler(const bool& val)
		{
			if ( update(lastValue, mLeft.value() >= mRight.value()) )
				announceUpdate(lastValue);
		}

	public:
		static_assert(Traits<T>::has_greater_equal, "T must support >= operator.");

		GreaterThanEqual(const std::shared_ptr<Watchable<T>>& left, const std::shared_ptr<Watchable<T>>& right)
		{
			mLeft.setFunction(std::bind(&GreaterThanEqual::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&GreaterThanEqual::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = (mLeft.value() >= mRight.value());
		}

		GreaterThanEqual(const T & left, const std::shared_ptr<Watchable<T>> & right)
			: GreaterThanEqual(std::make_shared<ConstWatchable<T>>(left), right)
		{
		}

		GreaterThanEqual(const std::shared_ptr<Watchable<T>> & left, const T & right)
			: GreaterThanEqual(left, std::make_shared<ConstWatchable<T>>(right))
		{
		}

		virtual bool value() const override
		{
			return (mLeft.value() >= mRight.value());
		}
	};

	template<typename T>
	class GreaterThan : public Watchable<bool>
	{
		bool lastValue;
		FunctionUpdateHandler<T> mLeft;
		FunctionUpdateHandler<T> mRight;

		void handler(const bool& val)
		{
			if ( update(lastValue, mLeft.value() > mRight.value()) )
				announceUpdate(lastValue);
		}

	public:
		static_assert(Traits<T>::has_greater_equal, "T must support >= operator.");

		GreaterThan(const std::shared_ptr<Watchable<T>>& left, const std::shared_ptr<Watchable<T>>& right)
		{
			mLeft.setFunction(std::bind(&GreaterThan::handler, this, std::placeholders::_1));
			mLeft.attach(left);

			mRight.setFunction(std::bind(&GreaterThan::handler, this, std::placeholders::_1));
			mRight.attach(right);

			lastValue = (mLeft.value() > mRight.value());
		}

		GreaterThan(const T& left, const std::shared_ptr<Watchable<T>>& right)
			: GreaterThan(std::make_shared<ConstWatchable<T>>(left), right)
		{
		}

		GreaterThan(const std::shared_ptr<Watchable<T>>& left, const T& right)
			: GreaterThan(left, std::make_shared<ConstWatchable<T>>(right))
		{
		}

		virtual bool value() const override
		{
			return (mLeft.value() > mRight.value());
		}
	};
}


#endif // !_STD_EXT_SIGNALS_LOGIC_H_
