#include "Task.h"

#include <functional>

namespace StdExt::Concurrent
{
	class STD_EXT_EXPORT FunctionTask : public Task
	{
	private:
		std::function<void()> mFunction;

	public:
		FunctionTask(std::function<void()>&& func);
		FunctionTask(const std::function<void()>& func);

	protected:
		virtual void run() override;
	};
}