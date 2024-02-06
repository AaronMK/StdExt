#include <StdExt/Concurrent/ThreadRunner.h>

#include <StdExt/Concurrent/Task.h>

namespace StdExt::Concurrent
{
	thread_local ThreadRunner* ThreadRunner::mActiveRunner = nullptr;

	bool ThreadRunner::isActive()
	{
		return (nullptr != mActiveRunner);
	}

	ThreadRunner::ThreadRunner(TaskBase* parent)
	{
		mTask = parent;
		mTask->mState = TaskState::InQueue;

		mThread = std::thread(
			[this]()
			{
				mActiveRunner = this;

				mTask->mState = TaskState::Running;
				mTask->run_task();
				mTask->mState = TaskState::Finished;

				mActiveRunner = nullptr;
			}
		);
	}
	
	ThreadRunner::~ThreadRunner()
	{
		mThread.join();
	}
}