#include <StdExt/Concurrent/Task.h>

#if defined(STD_EXT_APPLE)
#	include <Block.h>
#endif

#include <StdExt/Utility.h>
#include <StdExt/Exceptions.h>

namespace StdExt::Concurrent
{
	TaskState TaskBase::state() const
	{
		return mTaskState;
	}

	static void throwDormant()
	{
		throw invalid_operation("Attempting to wait on a dormant task.");
	}

	static void throwTimeout()
	{
		throw time_out("Task timed out while waiting for completion.");
	}

#if defined(STD_EXT_APPLE)
	TaskBase::TaskBase()
	{
		mTaskState     = TaskState::Dormant;
		mDispatchBlock = nullptr;
	}

	TaskBase::~TaskBase()
	{
		if ( TaskState::Dormant != mTaskState )
			internalWait();
	}

	void TaskBase::internalWait(Chrono::Milliseconds timeout)
	{
		TaskState task_state = mTaskState;

		if ( TaskState::Dormant == task_state )
			throwDormant();

		if ( TaskState::Finished == task_state )
			return;

		auto sys_timeout = ( timeout.count() > 0.0 ) ?
			dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(Chrono::Nanoseconds(timeout).count()) ) :
			dispatch_time(DISPATCH_TIME_FOREVER, 0);

		if ( 0 != dispatch_block_wait(mDispatchBlock, sys_timeout) )
			throwTimeout();

		auto cleanup = finalBlock(
			[this]()
			{
				Block_release(mDispatchBlock);
				mDispatchBlock = nullptr;
			}
		);

		if (mException)
			std::rethrow_exception(mException);
	}
#elif defined(STD_EXT_WIN32)
	TaskBase::TaskBase()
	{
		mTaskState     = TaskState::Dormant;
		mEvent.set();
	}

	TaskBase::~TaskBase()
	{
	}

	void TaskBase::internalWait(Chrono::Milliseconds timeout)
	{
		using namespace Chrono;

		TaskState task_state = mTaskState;

		if ( TaskState::Dormant == task_state )
			throwDormant();

		if ( TaskState::Finished == task_state )
			return;

		uint32_t sys_timeout = ( timeout.count() > 0.0 ) ? 
			static_cast<uint32_t>(Milliseconds(timeout).count()) :
			Concurrency::COOPERATIVE_TIMEOUT_INFINITE;

		if ( 0 != mEvent.wait(sys_timeout) )
			throwTimeout();

		if (mException)
			std::rethrow_exception(mException);
	}
#elif defined(STD_EXT_GCC)
	TaskBase::TaskBase()
	{
		mTaskState = TaskState::Dormant;
	}

	TaskBase::~TaskBase()
	{
	}

	void TaskBase::internalWait(Chrono::Milliseconds timeout)
	{
		using namespace std;
		
		TaskState task_state = mTaskState;

		if ( TaskState::Finished == task_state )
			return;

		if ( TaskState::Dormant == task_state || !mFuture.valid() )
			throwDormant();

		switch( mFuture.wait_for(timeout))
		{
		case future_status::ready:
			return;
		case future_status::deferred:
			throwDormant();
		case future_status::timeout:
			throwTimeout();
		}

		throw std::runtime_error("TaskBase::internalWait() - Unknown error. (GCC)");
	}
#endif
}
