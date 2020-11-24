#ifndef _STD_EXT_PLATFORM_H_
#define _STD_EXT_PLATFORM_H_

#include "StdExt.h"

namespace StdExt
{
	namespace Platform
	{
		namespace Compiler
		{
		#ifdef _MSC_VER
			static constexpr bool isVisualStudio = true;
			static constexpr bool isGcc = false;
			#ifdef _DEBUG
				static constexpr bool debug_build = true;
			#else
				static constexpr bool debug_build = false;
			#endif
		#else
			static constexpr bool isVisualStudio = false;
			static constexpr bool isGcc = true;
			#ifdef NDEBUG
				static constexpr bool debug_build = false;
			#else
				static constexpr bool debug_build = true;
			#endif
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
