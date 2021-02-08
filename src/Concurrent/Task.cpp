#include <StdExt/Concurrent/Task.h>

#include <StdExt/Concurrent/FunctionTask.h>
#include <StdExt/Concurrent/Queue.h>
#include <StdExt/Memory.h>

#include <thread>

namespace StdExt::Concurrent
{
	static Queue<Task*> subtask_queue;
	static Queue<Task*> main_task_queue;

	static const char* already_running_msg = "Attempting to schedule an already running task.";

	void Task::runTask(void* data)
	{
		Task* task_to_run = access_as<Task*>(data);
		
		if ( nullptr != task_to_run || subtask_queue.tryPop(task_to_run) || main_task_queue.tryPop(task_to_run) )
		{
			task_to_run->run();
			Task* parent_task = task_to_run->mParentTask;

			if ( 0 == --task_to_run->mDependentCount )
			{
				task_to_run->mParentTask = nullptr;
				task_to_run->mFinishedHandle.trigger();

				if ( task_to_run->mDeleteOnComplete )
					delete task_to_run;
			}

			if ( parent_task && 0 == --parent_task->mDependentCount )
			{
				parent_task->mParentTask = nullptr;
				parent_task->mFinishedHandle.trigger();

				if ( parent_task->mDeleteOnComplete )
					delete parent_task;
			}
		}
	}

	Task::Task()
	{
		mParentTask = nullptr;
		mDependentCount = 0;
		mFinishedHandle.trigger();
		mDeleteOnComplete = false;
	}

	Task::~Task()
	{
	}

	bool Task::isRunning() const
	{
		return !mFinishedHandle.isTriggered();
	}

	void Task::wait()
	{
		mFinishedHandle.wait();
	}

	void Task::runInline()
	{	
		if ( isRunning() )
			throw invalid_operation(already_running_msg);

		++mDependentCount;
		mFinishedHandle.reset();

		runTask(this);
	}

	void Task::runAsync()
	{
		if ( isRunning() )
			throw invalid_operation(already_running_msg);

		++mDependentCount;
		mFinishedHandle.reset();

		main_task_queue.push(this);

		#ifdef _WIN32
			Concurrency::CurrentScheduler::ScheduleTask(Task::runTask, nullptr);
		#else
		#	error "Need to implment Task::runAsync() for current platform."
		#endif // _WIN32
	}

	void Task::runAsThread()
	{
		if ( isRunning() )
			throw invalid_operation(already_running_msg);

		++mDependentCount;
		mFinishedHandle.reset();

		std::thread t( [this]() { runTask(this); } );
		t.detach();
	}

	void Task::subtask(Task* task)
	{
		if ( !isRunning() )
			throw invalid_operation("Subtasks can only be submitted when the parent task is running.");

		if ( task->isRunning() )
			throw invalid_operation(already_running_msg);

		++mDependentCount;

		++task->mDependentCount;
		task->mFinishedHandle.reset();
		task->mParentTask = this;

		subtask_queue.push(task);

		#ifdef _WIN32
			Concurrency::CurrentScheduler::ScheduleTask(Task::runTask, nullptr);
		#else
		#	error "Need to implment Task::runAsync() for current platform."
		#endif // _WIN32
	}

	void Task::subtask(std::function<void()>&& func)
	{
		FunctionTask* child_task = new FunctionTask(std::move(func));
		child_task->mDeleteOnComplete = true;

		subtask(child_task);
	}

	void Task::subtask(const std::function<void()>& func)
	{
		FunctionTask* child_task = new FunctionTask(func);
		child_task->mDeleteOnComplete = true;

		subtask(child_task);
	}
}