#ifndef _STD_EXT_PLATFORM_H_
#define _STD_EXT_PLATFORM_H_

namespace StdExt::Platform
{
	namespace Compiler
	{
		#ifdef _MSC_VER
		constexpr bool isVisualStudio = true;
		constexpr bool isGcc = false;
		#else
		static constexpr bool isVisualStudio = false;
		static constexpr bool isGcc = true;
		#endif
	}

#ifdef NDEBUG
	static constexpr bool isDebug = false;
#else
	static constexpr bool isDebug = true;
#endif
}


#endif // !_STD_EXT_PLATFORM_H_
