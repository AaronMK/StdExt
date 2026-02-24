#ifndef _STD_EXT_PLATFORM_H_
#define _STD_EXT_PLATFORM_H_

#include "StdExt.h"

#if defined(__apple_build_version__)
#	define STD_EXT_APPLE
#elif defined(_MSC_VER)
#	define STD_EXT_WIN32
#elif defined(__clang__)
#	define STD_EXT_CLANG
#else
#	define STD_EXT_GCC
#endif

#if !defined(STD_EXT_DEBUG)
#	if defined(_MSC_VER) && defined(_DEBUG)
#		define STD_EXT_DEBUG
#	elif defined(__clang__) && !defined(__OPTIMIZE__)
#		define STD_EXT_DEBUG
#	elif !defined(_MSC_VER) && !defined(__clang__) && !defined(NDEBUG)
#		define STD_EXT_DEBUG
#	endif
#endif

#if defined(_MSC_VER)
#	ifdef _CPPRTTI
#		define STD_EXT_RTTI
#	endif
#elif defined(__clang__)
#	if __has_feature(cxx_rtti)
#		define STD_EXT_RTTI
#	endif
#else
#	ifdef __GXX_RTTI
#		define STD_EXT_RTTI
#	endif
#endif

namespace StdExt
{
	namespace Platform
	{
		namespace Compiler
		{
		#if defined(STD_EXT_APPLE)
			static constexpr bool isVisualStudio = false;
			static constexpr bool isGcc = false;
			static constexpr bool isClang = true;
			static constexpr bool isApple = true;
		#elif defined(STD_EXT_WIN32)
			static constexpr bool isVisualStudio = true;
			static constexpr bool isGcc = false;
			static constexpr bool isClang = false;
			static constexpr bool isApple = false;
		#elif defined(STD_EXT_CLANG)
			static constexpr bool isVisualStudio = false;
			static constexpr bool isGcc = false;
			static constexpr bool isClang = true;
			static constexpr bool isApple = false;
		#else
			static constexpr bool isVisualStudio = false;
			static constexpr bool isGcc = true;
			static constexpr bool isClang = false;
			static constexpr bool isApple = false;
		#endif
		}
	}

	namespace Config
	{
		#if defined(STD_EXT_DEBUG)
			static constexpr bool Debug = true;
		#else
			static constexpr bool Debug = false;
		#endif

		#if defined(STD_EXT_RTTI)
			static constexpr bool RTTI = true;
		#else
			static constexpr bool RTTI = false;
		#endif
	}
}


#endif // !_STD_EXT_PLATFORM_H_
