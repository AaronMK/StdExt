#include <StdExt/Concurrent/Task.h>

#if defined(STD_EXT_APPLE)
#	include <Block.h>
#endif

#include <StdExt/Utility.h>

namespace StdExt::Concurrent
{
#if defined(STD_EXT_APPLE)
	TaskBase::TaskBase()
	{
		mTaskState     = TaskState::Dormant;
		mDispatchBlock = nullptr;
	}

	TaskBase::~TaskBase()
	{
		internalWait();
	}

	void TaskBase::internalWait(Chrono::Milliseconds timeout)
	{
		TaskState task_state = mTaskState;

		if ( TaskState::Dormant == task_state )
			throw invalid_operation("Attempting to wait on a dormant task.");

		if ( TaskState::Dormant == task_state || TaskState::Finished == task_state )
			return;

		auto sys_timeout = ( timeout.count() > 0.0 ) ?
			dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(Chrono::Nanoseconds(timeout).count()) ) :
			dispatch_time(DISPATCH_TIME_FOREVER, 0);

		if ( 0 != dispatch_block_wait(mDispatchBlock, sys_timeout) )
			throw time_out("Task timed out while waiting on get().");

		auto cleanup = finalBlock(
			[this]()
			{
				Block_release(mDispatchBlock);

				mDispatchBlock = nullptr;
				mTaskState     = TaskState::Dormant;
				mException     = std::exception_ptr();
			}
		);

		if (mException)
			std::rethrow_exception(mException);
	}
#endif
}
