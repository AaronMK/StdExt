#include <StdExt/Concurrent/Scheduler.h>

#if defined (STD_EXT_GCC)
#	include <StdExt/Concurrent/PredicatedCondition.h>

#	include <functional>
#	include <queue>
#	include <thread>
#endif

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
			throw invalid_operation("Tried to added an active task to a scheduler.");

		task->mTaskState = TaskState::InQueue;
		task->mException = std::exception_ptr();
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
#elif defined (STD_EXT_GCC)

	class Scheduler::SerialExecutor
	{
	public:
		using func_t = std::packaged_task<void()>;

		SerialExecutor()
		{
			mThread = std::thread(
				[&]()
				{
					execLoop();
				}
			);
		}

		~SerialExecutor()
		{
			mManager.destroy();
			mThread.join();
		}

		void addTask(func_t&& func)
		{
			mManager.trigger(
				[&]()
				{
					mTaskQueue.emplace( std::move(func) );
				}
			);
		}
	
	private:
		PredicatedCondition mManager;
		std::queue<func_t>  mTaskQueue;

		std::thread mThread;

		void execLoop()
		{
			try
			{
				while ( true )
				{
					func_t func_to_run;

					mManager.wait(
						[&]() -> bool
						{
							if ( mTaskQueue.size() )
							{
								func_to_run = std::move( mTaskQueue.front() );
								mTaskQueue.pop();

								return true;
							}

							return false;
						}
					);

					func_to_run();
				}
			}
			catch(const object_destroyed&)
			{
				while ( mTaskQueue.size() > 0 )
				{
					mTaskQueue.front()();
					mTaskQueue.pop();
				}
			}
		}
	};

	Scheduler::Scheduler(const String& name, SchedulerType stype)
		: mName(name)
	{
		if ( SchedulerType::SERIAL == stype)
			mSerialExecutor = std::make_unique<Scheduler::SerialExecutor>();
	}

	Scheduler::~Scheduler()
	{
	}

	void Scheduler::addTaskBase(TaskBase* task)
	{
		TaskState task_sate = task->mTaskState;

		if ( TaskState::InQueue == task_sate || TaskState::Running == task_sate )
			throw invalid_operation("Added an active task to a scheduler.");

		task->mTaskState = TaskState::InQueue;

		auto task_func = [task]() mutable
		{
			task->mTaskState = TaskState::Running;
			
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
		};

		if ( mSerialExecutor )
		{
			std::packaged_task<void()> pack_task( std::move(task_func) );
			task->mFuture = pack_task.get_future();

			mSerialExecutor->addTask( std::move(pack_task) );
		}
		else
			task->mFuture = std::async( std::launch::async, std::move(task_func) );
	}

#endif
}
