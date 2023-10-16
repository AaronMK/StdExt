#include <StdExt/Concurrent/TaskLoop.h>

#include <StdExt/Exceptions.h>

#include <cassert>

namespace StdExt::Concurrent
{
	TaskLoop::TaskLoop()
	{
	}

	TaskLoop::~TaskLoop()
	{
	}

	void TaskLoop::add(Task* task)
	{
		if (mEnd)
			throw invalid_operation("Cannot add task after loop has ended.");

		if (task->isRunning())
			throw invalid_operation("Cannot add running task to a TaskLoop.");


		++task->mDependentCount;
		task->mFinishedHandle.reset();

		mQueue.push(task);
		mAvailable.trigger();
	}

	void TaskLoop::end()
	{
		if (!mEnd)
		{
			mEnd.store(true);
			mAvailable.trigger();
		}
	}

	bool TaskLoop::canInline() const
	{
		return mEnd;
	}

	void TaskLoop::run()
	{
		while (true)
		{
			mAvailable.wait();
			mAvailable.reset();

			Task* task;
			while (mQueue.tryPop(task))
			{
				Task::runTask(task);
				task->wait();
			}

			if (mEnd)
			{
				mEnd.store(false);
				return;
			}
		}
	}
}
