#include <StdExt/Tasking/Task.h>

#include <StdExt/Tasking/SyncInterface.h>

#include "internal/ThreadPool.h"

namespace StdExt::Tasking
{
	Task::Task()
	{
	}

	Task::~Task()
	{
	}

	void Task::start()
	{
		using TP = ThreadPool;
		
		assert( !mRunner );

		mFinished.setValue(false);
		mRunner = makeSysTask(this);
	}

	size_t Task::waitForAny(std::initializer_list<Task*> task_list)
	{
		throw not_implemented("Task::waitForAny() not implmented.");
	}

	void Task::waitForAll(std::initializer_list<Task*> task_list)
	{
		throw not_implemented("Task::waitForAll() is not implemented.");
	}

	void Task::sync(SyncPoint& sync_point, const CallableArg<bool>& predicate)
	{
		sync_point.wait(predicate, [](WaitState state) {});
	}

	void sync(SyncPoint& sync_point, const CallableArg<bool>& predicate, const CallableArg<void, WaitState>& handler)
	{
	}

	SysTask Task::makeSysTask(Task* task)
	{
		task->run_task();
		co_return;
	}
}