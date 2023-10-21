#include <StdExt/Memory/Alignment.h>

#include <StdExt/Platform.h>
#include <StdExt/Utility.h>

#include <cstdlib>

#if defined(STD_EXT_APPLE)
#	include <malloc/malloc.h>
#endif

#if defined(STD_EXT_GCC)
#	include <malloc.h>
#endif

#ifdef _MSC_VER
#	include <cstring>
#endif

namespace StdExt::Memory
{
	void* alloc_aligned(size_t size, size_t alignment)
	{
		// apple-clang seems to have more strict parameter requirements.
		#if defined (STD_EXT_APPLE)
		alignment = nextMutltipleOf<size_t>(alignment, sizeof(void*));
		size = nextMutltipleOf<size_t>(size, alignment);
		#endif

		#if defined(STD_EXT_WIN32)
			return (size > 0) ? _aligned_malloc(size, alignment) : nullptr;
		#else
			return (size > 0) ? aligned_alloc(alignment, size) : nullptr;
		#endif
	}

	void free_aligned(void* ptr)
	{
		#if defined(STD_EXT_WIN32)
		if (nullptr != ptr)
			_aligned_free(ptr);
		#else
			free(ptr);
		#endif
	}

	void* realloc_aligned(void* ptr, size_t size, size_t alignment)
	{
	#if defined(STD_EXT_WIN32)
		if (nullptr != ptr)
			return _aligned_realloc(ptr, size, alignment);

		return nullptr;
	#elif defined (STD_EXT_APPLE)
		auto old_size = malloc_size(ptr);
		void* ret = alloc_aligned(size, alignment);
		memcpy(ret, ptr, std::min(old_size, size));
		free(ptr);
		
		return ret;
	#elif defined(STD_EXT_GCC)
		auto old_size = malloc_usable_size(ptr);
		void* ret = alloc_aligned(size, alignment);
		memcpy(ret, ptr, std::min(old_size, size));
		free(ptr);

		return ret;
	#endif
	}
}