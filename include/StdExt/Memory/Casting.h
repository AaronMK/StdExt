#ifndef _STD_EXT_MEMORY_CASTING_H_
#define _STD_EXT_MEMORY_CASTING_H_

#include "../Concepts.h"

namespace StdExt
{
	/**
	 * @brief
	 *  Casts a pointer, taking care of any necessary constant or other casting needed
	 *  to force the conversion to work.
	 */
	template<PointerType out_t, PointerType in_t>
	out_t force_cast_pointer(in_t ptr)
	{
		return reinterpret_cast<out_t>(
			const_cast<void*>(
				reinterpret_cast<const void*>(ptr)
			)
		);
	}

	/**
	 * @brief
	 *  For debug configurations, this will perform a checked cast and throw errors
	 *  upon failure.  For release configurations, this will be a simple quick
	 *  unchecked cast.
	 */
	template<PointerType out_t, PointerType in_t>
	out_t cast_pointer(in_t ptr)
	{
	#ifdef STD_EXT_DEBUG
		out_t ret = dynamic_cast<out_t>(ptr);
		
		if (ret == nullptr && ptr != nullptr)
			throw std::bad_cast();

		return ret;
	#else
		return reinterpret_cast<out_t>(ptr);
	#endif
	}

	template<PointerType T, PointerType ptr_t>
	T access_as(ptr_t data)
	{
		return force_cast_pointer<T>(data);
	}

	template<ReferenceType T, PointerType ptr_t>
	T access_as(ptr_t data)
	{
		using cast_t = std::add_pointer_t<
			std::remove_reference_t<T>
		>;

		return *force_cast_pointer<cast_t>(data);
	}
}

#endif // !_STD_EXT_MEMORY_CASTING_H_