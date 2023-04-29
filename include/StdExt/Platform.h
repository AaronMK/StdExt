#ifndef _STD_EXT_PLATFORM_H_
#define _STD_EXT_PLATFORM_H_

#include "StdExt.h"

#if defined(_MSC_VER)
#	define STD_EXT_WIN32
#elif defined(__apple_build_version__)
#	define STD_EXT_APPLE
#elif defined(__clang__)
#	define STD_DEF_CLANG
#else
#	define STD_EXT_GCC
#endif

namespace StdExt
{
	namespace Platform
	{
		namespace Compiler
		{
		#ifdef _MSC_VER
			static constexpr bool isVisualStudio = true;
			static constexpr bool isGcc = false;
			static constexpr bool isClang = false;
			#ifdef _DEBUG
				static constexpr bool debug_build = true;
			#else
				static constexpr bool debug_build = false;
			#endif
		#elif defined(__clang__)
			static constexpr bool isVisualStudio = true;
			static constexpr bool isGcc = false;
			static constexpr bool isClang = true;
			#ifdef __OPTIMIZE__
				static constexpr bool debug_build = false;
			#else
				static constexpr bool debug_build = true;
			#endif
		#else
			static constexpr bool isVisualStudio = false;
			static constexpr bool isGcc = true;
			static constexpr bool isClang = false;
			#ifdef NDEBUG
				static constexpr bool debug_build = false;
			#else
				static constexpr bool debug_build = true;
			#endif
		#endif

		#ifdef __apple_build_version__
			static constexpr bool isApple = true;
		#else
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
	}
}


#endif // !_STD_EXT_PLATFORM_H_
