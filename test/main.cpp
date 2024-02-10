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

static SyncPoint sync_point;
static int int_val;


class TestTask : public Task<void>
{

	int mTriggerVal;
	int mMyTriggerVal;

public:
	TestTask(int trigger_val)
		: mTriggerVal(trigger_val)
	{
	}

	void run() override
	{
		CombinedSyncInterface sync_interface(
			CallableSyncActions(
				[&]()
				{
					return (int_val == mTriggerVal);
				},
				[&]()
				{
					mMyTriggerVal = int_val;
				}
			),
			ThreadRunner::ThreadSync()
		);

		sync_point.wait(&sync_interface);
		sync_interface.Tasking.wait();
	}
};

int main()
{

	TestTask test_task_1(1);
	TestTask test_task_2(2);
	TestTask test_task_3(3);
	TestTask test_task_4(4);

	test_task_4.runAsThread();
	test_task_1.runAsThread();
	test_task_3.runAsThread();
	test_task_2.runAsThread();

	std::this_thread::sleep_for( std::chrono::seconds(3) );

	sync_point.trigger(
		[&]
		{
			int_val = 1;
		}
	);

	std::this_thread::sleep_for( std::chrono::seconds(3) );

	sync_point.trigger(
		[&]
		{
			int_val = 2;
		}
	);

	std::this_thread::sleep_for( std::chrono::seconds(3) );

	sync_point.trigger(
		[&]
		{
			int_val = 4;
		}
	);

	std::this_thread::sleep_for( std::chrono::seconds(3) );

	sync_point.destroy();

	test_task_1.wait();
	test_task_2.wait();
	test_task_3.wait();
	test_task_4.wait();

#if 0
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
#endif 0

	return 0;
}
