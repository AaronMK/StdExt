#ifndef _STD_EXT_SIGNALS_LOGIC_H_
#define _STD_EXT_SIGNALS_LOGIC_H_

#include "Watchable.h"
#include "FunctionHandlers.h"

#include "../Type.h"

#include <memory>

namespace StdExt
{
	class And : public Watchable<bool>
	{
	private:

		bool lastValue;
		FunctionUpdateHandler<bool> mLeft;
		FunctionUpdateHandler<bool> mRight;

	public:
		And(const std::shared_ptr<Watchable<bool>>& left, const std::shared_ptr<Watchable<bool>>& right)
		{
			auto handleFunc = [this](const bool& val)
			{
				if (lastValue != (mLeft.value() && mRight.value()))
				{
					lastValue = !lastValue;
					announceUpdate(lastValue);
				}
			};

			mLeft.setFunction(handleFunc);
			mLeft.attach(left);

			mRight.setFunction(handleFunc);
			mRight.attach(right);

			lastValue = (mLeft.value() && mRight.value());
		}

		virtual bool value() const override
		{
			return (mLeft.value() && mRight.value());
		}
	};

	class Or : public Watchable<bool>
	{
	private:

		bool lastValue;
		FunctionUpdateHandler<bool> mLeft;
		FunctionUpdateHandler<bool> mRight;

	public:
		Or(const std::shared_ptr<Watchable<bool>>& left, const std::shared_ptr<Watchable<bool>>& right)
		{
			auto handleFunc = [this](const bool& val)
			{
				if (lastValue != (mLeft.value() || mRight.value()))
				{
					lastValue = !lastValue;
					announceUpdate(lastValue);
				}
			};

			mLeft.setFunction(handleFunc);
			mLeft.attach(left);

			mRight.setFunction(handleFunc);
			mRight.attach(right);

			lastValue = (mLeft.value() || mRight.value());
		}

		virtual bool value() const override
		{
			return (mLeft.value() || mRight.value());
		}
	};

	class Xor : public Watchable<bool>
	{
	private:

		bool lastValue;
		FunctionUpdateHandler<bool> mLeft;
		FunctionUpdateHandler<bool> mRight;

	public:
		Xor(const std::shared_ptr<Watchable<bool>>& left, const std::shared_ptr<Watchable<bool>>& right)
		{
			auto handleFunc = [this](const bool& val)
			{
				if (lastValue != (mLeft.value() != mRight.value()))
				{
					lastValue = !lastValue;
					announceUpdate(lastValue);
				}
			};

			mLeft.setFunction(handleFunc);
			mLeft.attach(left);

			mRight.setFunction(handleFunc);
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

	public:
		Nor(const std::shared_ptr<Watchable<bool>>& left, const std::shared_ptr<Watchable<bool>>& right)
		{
			auto handleFunc = [this](const bool& val)
			{
				if (lastValue != (!mLeft.value() && !mRight.value()))
				{
					lastValue = !lastValue;
					announceUpdate(lastValue);
				}
			};

			mLeft.setFunction(handleFunc);
			mLeft.attach(left);

			mRight.setFunction(handleFunc);
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

	public:
		Not(const std::shared_ptr<Watchable<bool>>& input)
		{
			lastValue = input->value();
			auto handleFunc = [this](const bool& val)
			{
				if (lastValue != !mInput.value())
				{
					lastValue = !lastValue;
					announceUpdate(lastValue);
				}
			};

			mInput.setFunction(std::move(handleFunc));
			mInput.attach(input);

			lastValue = !mInput.value();
		}

		virtual bool value() const override
		{
			return !mInput.value();
		}
	};
}


#endif // !_STD_EXT_SIGNALS_LOGIC_H_
