#ifndef _STD_EXT_CONFIG_H_
#define _STD_EXT_CONFIG_H_

////////////////////////////////////////////////////////

#if !defined(STD_EXT_DYNAMIC) || !defined(_WIN32)
#	define STD_EXT_EXPORT
#else
#	ifdef STD_EXT_BUILD
#		define STD_EXT_EXPORT __declspec(dllexport)
#	else
#		define STD_EXT_EXPORT __declspec(dllimport)
#	endif
#endif

#endif // _STD_EXT_CONFIG_H_