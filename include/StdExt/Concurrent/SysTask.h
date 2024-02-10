#ifndef _STD_EXT_CONCURRENT_SYS_TASK_H_
#define _STD_EXT_CONCURRENT_SYS_TASK_H_

#include "Concurrent.h"
#include "SyncPoint.h"

#include "../Concepts.h"

#include <exception>

namespace StdExt::Concurrent
{
	class TaskBase;

#if defined(STD_EXT_COROUTINE_TASKS)
	class STD_EXT_EXPORT SysTask final
	{
		friend class Scheduler;

	public:
		class promise_type
		{
		public:
			promise_type(TaskBase* parent);

			std::suspend_always initial_suspend();
			std::suspend_always final_suspend() noexcept;

			void return_void();
			void unhandled_exception();

			SysTask get_return_object();

			std::coroutine_handle<promise_type> getHandle()
			{
				return std::coroutine_handle<promise_type>::from_promise(*this);
			}

			TaskBase*          mParent;
			std::exception_ptr mException;
		};

		using handle_t = std::coroutine_handle<promise_type>;

		SysTask(const SysTask&) = delete;
		SysTask& operator=(const SysTask&) = delete;

		SysTask(SysTask&& other);

		SysTask();
		~SysTask();

		SysTask& operator=(SysTask&& other);
		operator bool() const;

		static SysTask makeCoroutine(TaskBase* parent);

		const promise_type* getPromise() const;
		promise_type* getPromise();

		const TaskBase* getTask() const;
		TaskBase* getTask();

		bool isDone() const;
		void resume();

	private:
		SysTask(handle_t&& handle);

		handle_t mHandle;
	};

#elif defined(STD_EXT_WIN32)
	class STD_EXT_EXPORT SysTask : public concurrency::agent
	{
	public:
		SysTask(TaskBase* parent, concurrency::Scheduler& sys_scheduler);
		virtual ~SysTask();
	
	protected:
		virtual void run();

	private:
		TaskBase* mParent;
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