#include <StdExt/Concurrent/Scheduler.h>

namespace StdExt::Concurrent
{
#if defined (STD_EXT_APPLE)
	Scheduler::Scheduler(SchedulerType stype)
		: Scheduler(String::literal(u8""), stype)
	{
	}

	Scheduler::Scheduler(const String& name, SchedulerType stype)
	{
		StringBase<char> charStr;

		if ( name.size() > 0 )
			charStr = convertString<char>(name);

		const char* c_name = (charStr.size() > 0 ) ? charStr.data() : nullptr;

		dispatch_queue_attr_t queue_type = (stype == SchedulerType::PARALLEL ) ?
			DISPATCH_QUEUE_CONCURRENT : DISPATCH_QUEUE_SERIAL;

		auto char_str = convertString<char>(name);
		mDispatchQueue = dispatch_queue_create(c_name, queue_type);
	}

	Scheduler::~Scheduler()
	{
		dispatch_release(mDispatchQueue);
	}

	void Scheduler::addTaskBase(TaskBase* task)
	{
		if ( task->mDispatchBlock )
			throw invalid_operation("Added an active task to a scheduler.");

		task->mTaskState = TaskState::InQueue;
		task->mDispatchBlock = dispatch_block_create(
			dispatch_block_flags_t(0), ^()
			{
				try
				{
					task->mTaskState = TaskState::Running;
					task->schedulerRun();
				}
				catch (...)
				{
					task->mException = std::current_exception();
				}

				task->mTaskState = TaskState::Finished;
			}
		);

		dispatch_async(mDispatchQueue, task->mDispatchBlock);
	}
#endif
}
