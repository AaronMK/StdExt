#ifndef _STD_EXT_CONCURRENT_SYS_TASK_H_
#define _STD_EXT_CONCURRENT_SYS_TASK_H_

#include "Tasking.h"
#include "SyncPoint.h"

#include "../Concepts.h"

#include <exception>

namespace StdExt::Tasking
{
	class Task;


#if defined(STD_EXT_COROUTINE_TASKS)
	class STD_EXT_EXPORT SysTask final
	{
		friend class Scheduler;

	public:
		class promise_type
		{
		public:
			promise_type()
				: mParent(nullptr)
			{
			}

			promise_type(Task* parent)
				: mParent(parent)
			{
			}

			std::suspend_always initial_suspend();
			std::suspend_always final_suspend() noexcept;

			void return_void();
			void unhandled_exception();

			SysTask get_return_object();

			std::coroutine_handle<promise_type> getHandle();

			Task*          mParent;
			std::exception_ptr mException;
		};

		class SysSyncTasking : public SyncTasking
		{
		public:
			SysSyncTasking();
			virtual ~SysSyncTasking();

			void suspend() override;
			void wake()           override;
		};

		using handle_t = std::coroutine_handle<promise_type>;

		SysTask(const SysTask&) = delete;
		SysTask& operator=(const SysTask&) = delete;

		SysTask(SysTask&& other) noexcept;

		SysTask();
		~SysTask();

		SysTask& operator=(SysTask&& other);
		operator bool() const;

		const promise_type* getPromise() const;
		promise_type* getPromise();

		const Task* getTask() const;
		Task* getTask();

		const handle_t getHandle() const;
		handle_t getHandle();

		bool isDone() const;
		void resume();

	private:
		SysTask(handle_t handle);

		handle_t mHandle;
	};

#elif defined(STD_EXT_WIN32)
	class STD_EXT_EXPORT SysTask : public concurrency::agent
	{
	public:
		SysTask(Task* parent, concurrency::Scheduler& sys_scheduler);
		virtual ~SysTask();
	
	protected:
		virtual void run();

	private:
		Task* mParent;
	};
#elif defined(STD_EXT_APPLE)
	class SysTask
	{
	public:
		SysTask(dispatch_block_t db);
		virtual ~SysTask();

		dispatch_block_t mDispatchBlock;
	};
#endif
}

#endif // !_STD_EXT_CONCURRENT_SYS_TASK_H_