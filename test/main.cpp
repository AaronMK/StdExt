extern void testConst();
extern void testAny();
extern void testInPlace();
extern void testCollections();
extern void testCallableTraits();
extern void testCallable();
extern void testMemory();
extern void testSignals();
extern void testNumber();
extern void testVec();
extern void testMatrix();
extern void testConcurrent();
extern void testSerialize();
extern void testString();
extern void testTemplateUtility();
extern void testTypeInfo();
extern void testUtility();
extern void testIpComm();
extern void testStreams();

#include <StdExt/Concurrent/CallableTask.h>
#include <StdExt/Concurrent/ThreadRunner.h>
#include <StdExt/Concurrent/SyncPoint.h>

using namespace StdExt::Concurrent;

int main()
{
	int int_val = 0;
	SyncPoint sync_point;

	CallableTask test_task(
		[&]()
		{
			CombinedSyncInterface sync_interface(
				CallableSyncActions(
					[&]()
					{
						return (int_val > 0);
					},
					[&]()
					{
						++int_val;
					}
				),
				ThreadRunner::ThreadSync()
			);

			sync_point.wait(&sync_interface);
			sync_interface.Tasking.wait();
		}
	);

	test_task.runAsThread();

	std::this_thread::sleep_for( std::chrono::seconds(3) );

	sync_point.trigger(
		[&]
		{
			int_val = 1;
			return true;
		}
	);

	test_task.wait();

	testConcurrent();
	testIpComm();
	testTemplateUtility();
	testCallable();
	testCallableTraits();
	testString();
	testStreams();
	testString();
	testMemory();
	testConst();
	testUtility();
	testTypeInfo();
	testCollections();
	testVec();
	testSerialize();
	testMatrix();
	testNumber();
	testSignals();
	testInPlace();
	testAny();

	return 0;
}
