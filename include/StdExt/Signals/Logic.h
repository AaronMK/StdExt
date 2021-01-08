#ifndef _STD_EXT_SIGNALS_LOGIC_H_
#define _STD_EXT_SIGNALS_LOGIC_H_

#include "Watchable.h"
#include "Constant.h"
#include "Aggregator.h"
#include "FunctionHandlers.h"

#include "../Any.h"
#include "../Type.h"
#include "../Utility.h"
#include "../Concepts.h"

#include "../Collections/Vector.h"

#include <memory>


namespace StdExt::Signals
{
	class And : public Aggregator<bool>
	{
	protected:
		bool calcValue() const
		{
			for (size_t i = 0; i < size(); ++i)
			{
				if ( !valueAt(i) )
					return false;
			}

			return true;
		}

		virtual void onUpdate(size_t index, bool value)
		{
			updateValue(value && calcValue());
		}

	public:
		template<typename ...args_t>
			requires AssignableFrom<const Watchable<bool>&, args_t...>
		And(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			updateValue( calcValue() );
		}
	};

	class Or : public Aggregator<bool>
	{
	protected:
		bool calcValue() const
		{
			for (size_t i = 0; i < size(); ++i)
			{
				if ( valueAt(i) )
					return true;
			}

			return false;
		}

		virtual void onUpdate(size_t index, bool value)
		{
			updateValue(value || calcValue());
		}

	public:
		template<typename ...args_t>
			requires AssignableFrom<const Watchable<bool>&, args_t...>
		Or(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			updateValue( calcValue() );
		}
	};

	class Count : public Aggregator<bool, size_t>
	{
	protected:
		virtual void onUpdate(size_t index, bool value)
		{
			updateValue(value || calcValue());
		}

	public:
		template<typename ...args_t>
			requires AssignableFrom<const Watchable<bool>&, args_t...>
		Or(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			updateValue( calcValue() );
		}
	};





	class Count : public Watchable<int>, private Aggregator<bool>
	{
	private:
		virtual void onNotify(size_t index, bool evtValue)
		{
			notify(calcValue());
		}

	public:
		template<typename ...args_t>
		Count(args_t ...arguments)
			: Aggregator(std::forward<args_t>(arguments)...)
		{
			attach(std::forward<args_t>(arguments)...);
		}

		template<typename ...args_t>
			requires AssignableFrom<const Watchable<bool>&, args_t...>
		void attach(args_t ...arguments)
		{
			Aggregator::setInputs(std::forward<args_t>(arguments)...);
			notify(calcValue());
		}

	protected:
		virtual int calcValue()  const override
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
		using pass_t = typename FunctionPtrUpdateHandler<T>::pass_t;

		FunctionPtrUpdateHandler<T> mLeft;
		FunctionPtrUpdateHandler<T> mRight;

		void handler(pass_t val)
		{
			notify(calcValue());
		}

	public:

		void attach(const Watchable<T>& left, const Watchable<T>& right)
		{
			mLeft.blockUpdates(true);
			mRight.blockUpdates(true);

			mLeft.attach(left);
			mRight.attach(right);

			mLeft.blockUpdates(false);
			mRight.blockUpdates(false);

			notify(calcValue());
		}

	protected:

		virtual bool calcValue() const = 0;

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
			FunctionPtr<void, pass_t> handlerPtr(&Comparer::handler, this);

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
			FunctionPtr<void, pass_t> handlerPtr(&Comparer::handler, this);

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

	protected:
		virtual bool calcValue() const override
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

	protected:
		virtual bool calcValue() const override
		{
			return (!Left() && !Right());
		}
	};

	class Not : public Watchable<bool>
	{
	private:
		bool lastValue;
		FunctionPtrUpdateHandler<bool> mInput;

		void handler(bool val)
		{
			if ( update(lastValue, !val) )
				notify(lastValue);
		}

	public:
		Not(const Watchable<bool>& input)
		{
			FunctionPtr<void, bool> handlerPtr(&Not::handler, this);

			mInput.setFunction(handlerPtr);
			mInput.attach(input);

			lastValue = !mInput.value();
		}

		Not(Not&& other) noexcept
		{
			FunctionPtr<void, bool> handlerPtr(&Not::handler, this);

			mInput = std::move(other.mInput);
			mInput.setFunction(handlerPtr);

			lastValue = other.lastValue;
		}

		Not& operator=(Not&& other) noexcept
		{
			FunctionPtr<void, bool> handlerPtr(&Not::handler, this);

			mInput = std::move(other.mInput);
			mInput.setFunction(handlerPtr);

			lastValue = other.lastValue;

			return *this;
		}

	protected:
		virtual bool calcValue() const override
		{
			return !mInput.value();
		}
	};

	template<HasLessThan T>
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

	protected:
		virtual bool calcValue() const override
		{
			return (Left() < Right());
		}
	};

	template<HasLessThanEqual T>
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

	protected:
		virtual bool calcValue() const override
		{
			return (Left() <= Right());
		}
	};

	template<HasEquals T>
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

	protected:
		virtual bool calcValue() const override
		{
			if constexpr (std::is_floating_point_v<T>)
				return approximatelyEqual(Left(), Right());
			else
				return (Left() == Left());
		}
	};

	template<HasNotEqual T>
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

	protected:
		virtual bool calcValue() const override
		{
			if constexpr (std::is_floating_point_v<T>)
				return !approximatelyEqual(Left(), Right());
			else
				return (Left() != Left());
		}
	};

	template<HasGreaterThanEqual T>
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

	protected:
		virtual bool calcValue() const override
		{
			return (Left() >= Right());
		}
	};

	template<HasGreaterThan T>
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

	protected:
		virtual bool calcValue() const override
		{
			return (Left() > Right());
		}
	};
}

#endif // !_STD_EXT_SIGNALS_LOGIC_H_
