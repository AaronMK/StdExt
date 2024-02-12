#ifndef _STD_EXT_CONCURRENT_CONCURRENT_H_
#define _STD_EXT_CONCURRENT_CONCURRENT_H_

#include "../StdExt.h"

#define STD_EXT_FORCE_COROUTINE_TASKS

#if (!defined(STD_EXT_WIN32) && !defined(STD_EXT_APPLE)) || defined(STD_EXT_FORCE_COROUTINE_TASKS)
#	define  STD_EXT_COROUTINE_TASKS
#	include <coroutine>
#elif defined(STD_EXT_APPLE)
#	include <dispatch/dispatch.h>
#elif defined(STD_EXT_WIN32)
#	include <agents.h>
#	include <concrt.h>
#endif

#endif // !_STD_EXT_CONCURRENT_CONCURRENT_H_