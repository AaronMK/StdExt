#ifndef _STD_EXT_H_
#define _STD_EXT_H_

#if __has_include("Configuration.h")
#	include "Configuration.h"
#endif

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include "Platform.h"

////////////////////////////////////////////////////////

#if !defined(STD_EXT_DYNAMIC) || !defined(STD_EXT_WIN32)
#	define STD_EXT_EXPORT
#else
#	ifdef STD_EXT_BUILD
#		define STD_EXT_EXPORT __declspec(dllexport)
#	else
#		define STD_EXT_EXPORT __declspec(dllimport)
#	endif
#endif

/**
 * @brief
 *  A collection of classes that are of common use that are either absent from the c++ standard
 *  library, or replacements for standard library functionality that offer a notable
 *  improvement.
 */
namespace StdExt
{
}

#endif // _STD_EXT_H_