#include <StdExt/Concurrent/ThreadRunner.h>

#include <StdExt/Concurrent/Task.h>

namespace StdExt::Concurrent
{
	thread_local ThreadRunner* ThreadRunner::mActiveRunner = nullptr;

	bool ThreadRunner::isActive()
	{
		return (nullptr != mActiveRunner);
	}

	ThreadRunner::ThreadSync::ThreadSync()
		: mAtomicFlag(mActiveRunner->mFlag)
	{
		mAtomicFlag.test_and_set();
	};

	ThreadRunner::ThreadSync::~ThreadSync()
	{
	}

	void ThreadRunner::ThreadSync::markForSuspend()
	{
		mAtomicFlag.clear();
	}
			
	void ThreadRunner::ThreadSync:: wake()
	{
		mAtomicFlag.test_and_set();
		mAtomicFlag.notify_one();
	}

	void ThreadRunner::ThreadSync::wait()
	{
		mAtomicFlag.wait(true);
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

				mActiveRunner = nullptr;
			}
		);
	}
	
	ThreadRunner::~ThreadRunner()
	{
		mThread.join();
	}
}