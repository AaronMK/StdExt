#include <StdExt/Signals/Event.h>
#include <StdExt/Signals/Invokable.h>
#include <StdExt/Signals/Subscription.h>
#include <StdExt/Signals/Settable.h>

#include <StdExt/Test/Test.h>

#include <string>
#include <vector>
#include <iostream>
#include <initializer_list>

using namespace StdExt::Signals;
using namespace StdExt::Test;
using namespace StdExt;

using namespace std;

class MarkHandler : public EventHandler<int>
{
private:
	int* mMarkLocation = nullptr;

public:
	MarkHandler(MarkHandler&&) = default;
	MarkHandler& operator=(MarkHandler&&) = default;
	
	MarkHandler(int* target) noexcept
		: mMarkLocation(target)
	{

	}

protected:
	virtual void handleEvent(int evt_val) override
	{
		*mMarkLocation += evt_val;
	}
};

template<typename T>
class MarkerSubscription : public Subscription<T>
{
	T* mMarker;
public:
	MarkerSubscription(T* marker)
		: mMarker(marker)
	{
	}

protected:
	virtual void onDetached() override
	{
		*mMarker = T{};
	}

	virtual void onUpdated(Subscription<T>::pass_t new_value)
	{
		*mMarker = new_value;
	}
};

void testSignals()
{

#	pragma region Event/EventHandler
	{
		std::vector<MarkHandler> handlers;
		std::array<int, 5> markers = {0, 0, 0, 0, 0};

		Invokable<int> intEvent;

		for ( size_t i = 0; i < markers.size(); ++i )
		{
			handlers.emplace_back(&markers[i]);
			handlers[i].bind(intEvent);
		}

		testByCheck(
			"All event handlers are called.",
			[&]()
			{
				intEvent.invoke(1);
			},
			[&]()
			{
				return
					markers[0] == 1 &&
					markers[1] == 1 &&
					markers[2] == 1 &&
					markers[3] == 1 &&
					markers[4] == 1;
			}
		);

		Invokable<int> intEvent_next = std::move(intEvent);

		testByCheck(
			"Event that was target for a move triggers events.",
			[&]()
			{
				intEvent_next.invoke(1);
			},
			[&]()
			{
				return
					markers[0] == 2 &&
					markers[1] == 2 &&
					markers[2] == 2 &&
					markers[3] == 2 &&
					markers[4] == 2;
			}
		);

		testByCheck(
			"Events can unbind.",
			[&]()
			{
				handlers[1].unbind();
				handlers[3].unbind();
				intEvent_next.invoke(1);
			},
			[&]()
			{
				return
					markers[0] == 3 &&
					markers[1] == 2 &&
					markers[2] == 3 &&
					markers[3] == 2 &&
					markers[4] == 3;
			}
		);

		testByCheck(
			"Moved handler is called, and invalidated handler is not.",
			[&]()
			{
				MarkHandler handler_moved = std::move(handlers[0]);
				intEvent_next.invoke(1);
			},
			[&]()
			{
				return
					markers[0] == 4 &&
					markers[1] == 2 &&
					markers[2] == 4 &&
					markers[3] == 2 &&
					markers[4] == 4;
			}
		);

		testByCheck(
			"Destroyed handler is not called.",
			[&]()
			{
				intEvent_next.invoke(1);
			},
			[&]()
			{
				return
					markers[0] == 4 &&
					markers[1] == 2 &&
					markers[2] == 5 &&
					markers[3] == 2 &&
					markers[4] == 5;
			}
		);
	}
#	pragma endregion

#	pragma region Watchable/Subscription
	{
		const char* strUninitialized = "<Uninitialized>";

		std::vector<MarkerSubscription<int>> intSubs;
		std::vector<MarkerSubscription<std::string>> stringSubs;

		std::array<int, 4> intMarkers{};
		std::fill(intMarkers.begin(), intMarkers.end(), -1);
		
		std::array<std::string, 4> stringMarkers;
		std::fill(stringMarkers.begin(), stringMarkers.end(), strUninitialized);

		intSubs.emplace_back(&intMarkers[0]);
		intSubs.emplace_back(&intMarkers[1]);
		intSubs.emplace_back(&intMarkers[2]);
		intSubs.emplace_back(&intMarkers[3]);

		stringSubs.emplace_back(&stringMarkers[0]);
		stringSubs.emplace_back(&stringMarkers[1]);
		stringSubs.emplace_back(&stringMarkers[2]);
		stringSubs.emplace_back(&stringMarkers[3]);

		Settable<int> watchedInt;
		Settable<std::string> watchedString;

		intSubs[1].attach(watchedInt);
		intSubs[2].attach(watchedInt);
		intSubs[3].attach(watchedInt);

		stringSubs[1].attach(watchedString);
		stringSubs[2].attach(watchedString);
		stringSubs[3].attach(watchedString);

		testForException<invalid_operation>(
			"Detached subscription raises exception when a value operation is attempted. (primitive)",
			[&]()
			{
				intSubs[0].value();
			}
		);
		
		testForException<invalid_operation>(
			"Detached subscription raises exception when a value operation is attempted. (class)",
			[&]()
			{
				stringSubs[0].value();
			}
		);

		watchedInt.setValue(2);

		testByCheck(
			"Subscriptions reflect updated watched value. (primitive)",
			[&]()
			{
				return
					intSubs[1].value() == 2 &&
					intSubs[2].value() == 2 &&
					intSubs[3].value() == 2;
			}
		);
		
		testByCheck(
			"Attached Subscriptions called onUpdated() override. (primitive)",
			[&]()
			{
				return
					intMarkers[0] == -1 &&
					intMarkers[1] == 2 &&
					intMarkers[2] == 2 &&
					intMarkers[3] == 2;
			}
		);
		
		watchedString.setValue("two");

		testByCheck(
			"Subscriptions reflect updated watched value. (class)",
			[&]()
			{
				return
					stringSubs[1].value().compare("two") == 0 &&
					stringSubs[2].value().compare("two") == 0 &&
					stringSubs[3].value().compare("two") == 0;
			}
		);

		testByCheck(
			"Attached Subscriptions called onUpdated() override. (class)",
			[&]()
			{
				return
					stringMarkers[0].compare(strUninitialized) == 0 &&
					stringMarkers[1].compare("two") == 0 &&
					stringMarkers[2].compare("two") == 0 &&
					stringMarkers[3].compare("two") == 0;
			}
		);

		intSubs[0].attach(watchedInt);

		testForResult<int>(
			"Attaching causes onUpdated() override to be called. (primitive)",
			2, intMarkers[0]
		);

		stringSubs[0].attach(watchedString);

		testForResult<int>(
			"Attaching causes onUpdated() override to be called. (class)",
			0, stringMarkers[0].compare("two")
		);

		intSubs[0].detach();
		intSubs[1].detach();

		testByCheck(
			"Attached Subscriptions called onDetached() override. (primitive)",
			[&]()
			{
				return
					intMarkers[0] == 0 &&
					intMarkers[1] == 0 &&
					intMarkers[2] == 2 &&
					intMarkers[3] == 2;
			}
		);

		stringSubs[0].detach();
		stringSubs[1].detach();

		testByCheck(
			"Attached Subscriptions called onDetached() override. (class)",
			[&]()
			{
				return
					stringMarkers[0].compare("") == 0 &&
					stringMarkers[1].compare("") == 0 &&
					stringMarkers[2].compare("two") == 0 &&
					stringMarkers[3].compare("two") == 0;
			}
		);

		testByCheck(
			"Subscriptions properly report attachment status.",
			[&]()
			{
				return
					!intSubs[0].isAttached() &&
					!intSubs[1].isAttached() &&
					 intSubs[2].isAttached() &&
					 intSubs[3].isAttached() &&
					!stringSubs[0].isAttached() &&
					!stringSubs[1].isAttached() &&
					 stringSubs[2].isAttached() &&
					 stringSubs[3].isAttached();
			}
		);
	}
#	pragma endregion
}