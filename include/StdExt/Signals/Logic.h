#ifndef _STD_EXT_SIGNALS_LOGIC_H_
#define _STD_EXT_SIGNALS_LOGIC_H_

#include "Watchable.h"
#include "Constant.h"
#include "Aggregator.h"
#include "FunctionHandlers.h"

#include "../Any.h"
#include "../Type.h"
#include "../Utility.h"

#include "../Collections/Vector.h"

#include <memory>


namespace StdExt::Signals
{
	class And : public Watchable<bool>, private Aggregator<bool>
	{
	private:
		virtual void onNotify(size_t index, bool evtValue)
		{
			notify( (!evtValue) ? false : value() );
		}

	public:
		template<typename ...args_t>
		And(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			attach(std::forward<args_t>(arguments)...);
		}

		template<typename ...args_t>
		void attach(args_t ...arguments)
		{
			static_assert(can_assign_from_v<const Watchable<bool>&, args_t...>,
				"All arguments must be convertable to const Watchable<bool>&.");

			Aggregator::setInputs(std::forward<args_t>(arguments)...);
			notify(value());
		}

		virtual bool value() const override
		{
			for (size_t i = 0; i < size(); ++i)
			{
				if ( !dynamic_cast<const Watchable<bool>*>(&(*this)[i])->value() )
					return false;
			}

			return true;
		}
	};

	class Or : public Watchable<bool>, private Aggregator<bool>
	{
	private:
		virtual void onNotify(size_t index, bool evtValue)
		{
			notify(evtValue || value());
		}

	public:
		template<typename ...args_t>
		Or(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			attach(std::forward<args_t>(arguments)...);
		}

		template<typename ...args_t>
		void attach(args_t ...arguments)
		{
			static_assert(can_assign_from_v<const Watchable<bool>&, args_t...>,
				"All arguments must be convertable to const Watchable<bool>&.");

			Aggregator::setInputs(std::forward<args_t>(arguments)...);
			notify(value());
		}

		virtual bool value() const override
		{
			for (size_t i = 0; i < size(); ++i)
			{
				if (dynamic_cast<const Watchable<bool>*>(&(*this)[i])->value())
					return true;
			}

			return false;
		}
	};

	class Count : public Watchable<int>, private Aggregator<bool>
	{
	private:
		virtual void onNotify(size_t index, bool evtValue)
		{
			notify(value());
		}

	public:
		template<typename ...args_t>
		Count(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			attach(std::forward<args_t>(arguments)...);
		}

		template<typename ...args_t>
		void attach(args_t ...arguments)
		{
			static_assert(can_assign_from_v<const Watchable<bool>&, args_t...>,
				"All arguments must be convertable to const Watchable<bool>&.");

			Aggregator::setInputs(std::forward<args_t>(arguments)...);
			notify(value());
		}

		virtual int value() const override
		{
			int ret = 0;
			for (size_t i = 0; i < size(); ++i)
			{
				if (dynamic_cast<const Watchable<bool>*>(&(*this)[i])->value())
					++ret;
			}

			return ret;
		}
	};

	template<typename T>
	class Comparer : public Watchable<bool>
	{

	private:
		FunctionPtrUpdateHandler<T> mLeft;
		FunctionPtrUpdateHandler<T> mRight;

		void handler(const T& val)
		{
			notify(value());
		}

	public:
		virtual bool value() const = 0;

		void attach(const Watchable<bool>& left, const Watchable<bool>& right)
		{
			mLeft.attach(left);
			mRight.attach(right);

			notify(value());
		}

	protected:
		inline T Left() const
		{
			return mLeft.value();
		}

		inline T Right() const
		{
			return mRight.value();
		}

		Comparer()
		{
			FunctionPtr<void, const T&> handlerPtr(&Comparer::handler, this);

			mLeft.setFunction(handlerPtr);
			mRight.setFunction(handlerPtr);
		}

		Comparer(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer()
		{
			attach(left, right);
		}

		Comparer(Comparer&& other) noexcept
			: Watchable<bool>(std::move(other))
		{
			FunctionPtr<void, const bool&> handlerPtr(&Comparer::handler, this);

			mLeft = std::move(other.mLeft);
			mLeft.setFunction(handlerPtr);

			mRight = std::move(other.mLeft);
			mRight.setFunction(handlerPtr);
		}

		Comparer& operator=(Comparer&& other) noexcept
		{
			Comparer(std::move(other));
			return *this;
		}
	};

	class Xor : public Comparer<bool>
	{

	public:
		Xor()
			: Comparer<bool>()
		{
		}

		Xor(const Watchable<bool>& left, const Watchable<bool>& right)
			: Comparer<bool>(left, right)
		{
		}

		virtual bool value() const override
		{
			return (Left() != Right());
		}
	};

	class Nor : public Comparer<bool>
	{
	public:
		Nor()
			: Comparer<bool>()
		{
		}

		Nor(const Watchable<bool>& left, const Watchable<bool>& right)
			: Comparer<bool>(left, right)
		{
		}

		virtual bool value() const override
		{
			return (!Left() && !Right());
		}
	};

	class Not : public Watchable<bool>
	{
	private:
		bool lastValue;
		FunctionPtrUpdateHandler<bool> mInput;

		void handler(const bool& val)
		{
			if ( update(lastValue, !val) )
				notify(lastValue);
		}

	public:
		Not(const Watchable<bool>& input)
		{
			FunctionPtr<void, const bool&> handlerPtr(&Not::handler, this);

			mInput.setFunction(handlerPtr);
			mInput.attach(input);

			lastValue = !mInput.value();
		}

		Not(Not&& other) noexcept
		{
			FunctionPtr<void, const bool&> handlerPtr(&Not::handler, this);

			mInput = std::move(other.mInput);
			mInput.setFunction(handlerPtr);

			lastValue = other.lastValue;
		}

		Not& operator=(Not&& other) noexcept
		{
			FunctionPtr<void, const bool&> handlerPtr(&Not::handler, this);

			mInput = std::move(other.mInput);
			mInput.setFunction(handlerPtr);

			lastValue = other.lastValue;

			return *this;
		}

		virtual bool value() const override
		{
			return !mInput.value();
		}
	};

	template<typename T>
	class LessThan : public Comparer<T>
	{

	public:
		static_assert(Traits<T>::has_less_than, "T must support < operator.");

		LessThan()
			: Comparer<T>()
		{
		}

		LessThan(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer<T>(left, right)
		{
		}

		virtual bool value() const override
		{
			return (Left() < Right());
		}
	};

	template<typename T>
	class LessThanEqual : public Comparer<T>
	{

	public:
		static_assert(Traits<T>::has_less_equal, "T must support <= operator.");

		LessThanEqual()
			: Comparer<T>()
		{
		}

		LessThanEqual(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer<T>(left, right)
		{
		}

		virtual bool value() const override
		{
			return (Left() <= Right());
		}
	};

	template<typename T>
	class Equal : public Comparer<T>
	{

	public:
		static_assert(Traits<T>::has_equality, "T must support == operator.");
		
		Equal()
			: Comparer<T>()
		{
		}

		Equal(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer<T>(left, right)
		{
		}

		virtual bool value() const override
		{
			if constexpr (std::is_floating_point_v<T>)
				return approximately_equal(Left(), Right());
			else
				return (Left() == Left());
		}
	};

	template<typename T>
	class NotEqual : public Comparer<T>
	{

	public:
		static_assert(Traits<T>::has_inequality, "T must support != operator.");
		
		NotEqual()
			: Comparer<T>()
		{
		}

		NotEqual(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer<T>(left, right)
		{
		}

		virtual bool value() const override
		{
			if constexpr (std::is_floating_point_v<T>)
				return !approximately_equal(Left(), Right());
			else
				return (Left() != Left());
		}
	};

	template<typename T>
	class GreaterThanEqual : public Comparer<T>
	{

	public:
		static_assert(Traits<T>::has_greater_equal, "T must support >= operator.");
		
		GreaterThanEqual()
			: Comparer<T>()
		{
		}
		
		GreaterThanEqual(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer<T>(left, right)
		{
		}

		virtual bool value() const override
		{
			return (Left() >= Right());
		}
	};

	template<typename T>
	class GreaterThan : public Comparer<T>
	{

	public:
		static_assert(Traits<T>::has_greater_than, "T must support > operator.");

		GreaterThan()
			: Comparer<T>()
		{
		}

		GreaterThan(const Watchable<T>& left, const Watchable<T>& right)
			: Comparer<T>(left, right)
		{
		}

		virtual bool value() const override
		{
			return (Left() > Right());
		}
	};
}

#endif // !_STD_EXT_SIGNALS_LOGIC_H_
