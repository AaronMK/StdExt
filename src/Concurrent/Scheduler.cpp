#include <StdExt/Concurrent/Scheduler.h>

namespace StdExt::Concurrent
{
	Scheduler::Scheduler(SchedulerType stype)
		: Scheduler(String::literal(u8""), stype)
	{
	}

#if defined (STD_EXT_APPLE)
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
#elif defined (STD_EXT_WIN32)
	Scheduler::Scheduler(const String& name, SchedulerType stype)
		: mName(name)
	{
		using namespace Concurrency;

		Concurrency::SchedulerPolicy policy;
		policy.SetPolicyValue(SchedulingProtocol, EnhanceScheduleGroupLocality);

		if ( SchedulerType::SERIAL == stype )
			policy.SetConcurrencyLimits(0, 1);

		mScheduler = Concurrency::Scheduler::Create(policy);
	}

	Scheduler::~Scheduler()
	{
		mScheduler->Release();
		mScheduler = nullptr;
	}

	void Scheduler::addTaskBase(TaskBase* task)
	{
		TaskState task_sate = task->mTaskState;

		if ( TaskState::InQueue == task_sate || TaskState::Running == task_sate )
			throw invalid_operation("Added an active task to a scheduler.");

		task->mEvent.reset();
		task->mException = std::exception_ptr();
		task->mTaskState = TaskState::InQueue;

		mScheduler->ScheduleTask(&runTask, task);
	}

	void Scheduler::runTask(void* task_ptr)
	{
		TaskBase* task = reinterpret_cast<TaskBase*>(task_ptr);

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
		task->mEvent.set();
	}
#endif
}
