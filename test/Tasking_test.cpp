#include <StdExt/Tasking/CallableTask.h>
#include <StdExt/Tasking/SyncInterface.h>

#include <chrono>

using namespace StdExt::Tasking;
using namespace std::chrono;

void testTasking()
{
	SyncPoint sync_point;
	int sync_value = 0;

	auto wait_for_one_and_three = CallableTask(
		[&]()
		{
			auto WaitOne = mixSyncActions<AtomicTaskSync>(
				CallableSyncActions(
					[&]()
					{
						return (1 == sync_value);
					}
				)
			);
			
			sync_point.wait(&WaitOne);
			WaitOne.clientWait();

			std::this_thread::sleep_for( seconds(3) );

			sync_point.trigger(
				[&]()
				{
					sync_value = 2;
				}
			);

			auto WaitThree = mixSyncActions<AtomicTaskSync>(
				CallableSyncActions(
					[&]()
					{
						return (3 == sync_value);
					}
				)
			);

			sync_point.wait(&WaitThree);
			WaitOne.clientWait();
		}
	);

	auto wait_for_two = CallableTask(
		[&]()
		{
			auto WaitTwo = mixSyncActions<AtomicTaskSync>(
				CallableSyncActions(
					[&]()
					{
						return (2 == sync_value);
					}
				)
			);
			
			sync_point.wait(&WaitTwo);
			WaitTwo.clientWait();
		}
	);

	wait_for_two.start();
	wait_for_one_and_three.start();

	std::this_thread::sleep_for( seconds(3) );

	sync_point.trigger(
		[&]
		{
			sync_value = 1;
		}
	);

	Task::waitForAll( {&wait_for_two, &wait_for_one_and_three});
}