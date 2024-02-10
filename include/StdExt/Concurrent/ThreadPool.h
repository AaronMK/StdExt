#ifndef _STD_EXT_CONCURRENT_THREAD_POOL_H_
#define _STD_EXT_CONCURRENT_THREAD_POOL_H_

#include "Concurrent.h"

#include <set>
#include <queue>
#include <vector>

namespace StdExt::Concurrent
{
#ifdef STD_EXT_COROUTINE_TASKS
	class SysTask;

	class STD_EXT_EXPORT ThreadPool
	{
		friend class SysTask;

	private:
		static std::set<SysTask*>   Blocked;
		static std::queue<SysTask*> Ready;

	public:

	};
#endif
}

#endif // !_STD_EXT_CONCURRENT_THREAD_POOL_H_