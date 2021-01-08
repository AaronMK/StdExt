#include <StdExt/Signals/Settable.h>
#include <StdExt/Signals/FunctionHandlers.h>
// #include <StdExt/Signals/Aggregator.h>
// #include <StdExt/Signals/Logic.h>

#include <StdExt/Concepts.h>
#include <StdExt/ParamPack.h>

#include <string>
#include <iostream>

using namespace StdExt::Signals;
using namespace std;

#if 0
class SRLatch
{
private:
	Settable<bool> mS;
	Settable<bool> mR;

	Nor mNorR;
	Nor mNorS;

public:
	SRLatch()
	{
		mNorR.attach(mR, mNorS);
		mNorR.attach(mS, mNorR);

	}

	Settable<bool>& S()
	{
		return mS;
	}

	const Settable<bool>& S() const
	{
		return mS;
	}

	Settable<bool>& R()
	{
		return mR;
	}

	const Settable<bool>& R() const
	{
		return mR;
	}

	const Watchable<bool>& Q()
	{
		return mNorR;
	}

	const Watchable<bool>& Qp()
	{
		return mNorS;
	}
};

class Count : public Aggregator<std::string, size_t>
{
private:
	size_t calcValue()
	{
		size_t count = 0;
		forEachSub(
			[&](const std::string& item)
			{
				count += (item.size() > 0) ? 1 : 0;
			}
		);

		return count;
	}

protected:
	virtual void onUpdate(size_t index, WatchablePassType<std::string> value) override
	{
		updateValue( calcValue() );
	}

public:
	
	Count(std::initializer_list<const Watchable<std::string>&> args)
		: Aggregator(args)
	{
		updateValue( calcValue() );
	}
};
#endif

template<typename ...args_t>
	requires AssignableFrom<const Watchable<std::string>&, args_t...>
void ParamTest(args_t ...arguments)
{
	int i = 1;
}

constexpr bool c_test = PointerType<const std::string, std::string*>;

void testSignals()
{
	Settable<std::string> stringA;
	Settable<std::string> stringB;

	std::string str("test");
	const std::string& strRef(str);

	// Count aggr({stringA, stringB});

	FunctionUpdateHandler<std::string> handlerA1(
		stringA,
		[](const std::string& str)
		{
			cout << "Handler A1(" << str << ")" << endl;
		}
	);

	FunctionUpdateHandler<std::string> handlerA2(
		stringB,
		[](const std::string& str)
		{
			cout << "Handler AB(" << str << ")" << endl;
		}
	);

	stringA.setValue("Setting String A");


}