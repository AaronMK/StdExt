#include <StdExt/Concurrent/FunctionTask.h>

namespace StdExt::Concurrent
{
	FunctionTask::FunctionTask(std::function<void()>&& func)
	{
		mFunction = std::move(func);
	}

	FunctionTask::FunctionTask(const std::function<void()>& func)
	{
		mFunction = func;
	}

	void FunctionTask::run()
	{
		mFunction();
	}
}