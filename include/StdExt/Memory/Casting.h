#ifndef _STD_EXT_MEMORY_CASTING_H_
#define _STD_EXT_MEMORY_CASTING_H_

#include "../Concepts.h"

namespace StdExt
{
	/**
	 * @brief
	 *  Casts a pointer, taking care of any necessary constant or other casting needed
	 *  to force the conversion to work.  Parameters are assumed to never be function
	 *  pointers, which allows the underlying conversions to go through void* safely.
	 *  When out_t and in_t point to the same type, differing only by constness, the
	 *  cast is a constant expression.
	 */
	template<DataPointerType out_t, DataPointerType in_t>
	constexpr out_t force_cast_pointer(in_t ptr)
	{
		if constexpr ( std::is_same_v<
				std::remove_cv_t<std::remove_pointer_t<out_t>>,
				std::remove_cv_t<std::remove_pointer_t<in_t>>
			> )
		{
			return const_cast<out_t>(ptr);
		}
		else
		{
			return reinterpret_cast<out_t>(
				const_cast<void*>(
					reinterpret_cast<const void*>(ptr)
				)
			);
		}
	}

	/**
	 * @brief
	 *  For debug configurations, this will perform a checked cast and throw errors
	 *  upon failure.  For release configurations, this will be a simple quick
	 *  unchecked cast.  Parameters are assumed to never be function pointers.  When
	 *  out_t and in_t point to the same type, differing only by constness, no runtime
	 *  check is needed and the cast is a constant expression.
	 */
	template<DataPointerType out_t, DataPointerType in_t>
	constexpr out_t cast_pointer(in_t ptr)
	{
		if constexpr ( std::is_same_v<
				std::remove_cv_t<std::remove_pointer_t<out_t>>,
				std::remove_cv_t<std::remove_pointer_t<in_t>>
			> )
		{
			return const_cast<out_t>(ptr);
		}
		else
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
	}

	template<DataPointerType T, DataPointerType ptr_t>
	constexpr T access_as(ptr_t data)
	{
		return force_cast_pointer<T>(data);
	}

	template<ReferenceType T, DataPointerType ptr_t>
	constexpr T access_as(ptr_t data)
	{
		using cast_t = std::add_pointer_t<
			std::remove_reference_t<T>
		>;

		return *force_cast_pointer<cast_t>(data);
	}
}

#endif // !_STD_EXT_MEMORY_CASTING_H_