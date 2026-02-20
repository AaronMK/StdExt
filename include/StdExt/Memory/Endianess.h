#ifndef _STD_EXT_MEMORY_ENDIANESS_H_
#define _STD_EXT_MEMORY_ENDIANESS_H_

#include "../Concepts.h"

#include <bit>

namespace StdExt
{
	template<Scalar T>
	T swap_endianness(T value)
	{
		if constexpr ( sizeof(T) == 1 )
		{
			return value;
		}
		else if constexpr ( sizeof(T) == 2 )
		{
			uint16_t op_val = access_as<uint16_t&>(&value);

			#ifdef _WIN32
				op_val = _byteswap_ushort(op_val);
			#else
				op_val = __builtin_bswap16(op_val);
			#endif
			
			return access_as<T&>(&op_val);
		}
		else if constexpr ( sizeof(T) == 4 )
		{
			uint32_t op_val = access_as<uint32_t&>(&value);

			#ifdef _WIN32
				op_val = _byteswap_ulong(op_val);
			#else
				op_val = __builtin_bswap32(op_val);
			#endif
			
			return access_as<T&>(&op_val);
		}
		else if constexpr ( sizeof(T) == 8 )
		{
			uint64_t op_val = access_as<uint64_t&>(&value);

			#ifdef _WIN32
				op_val = _byteswap_uint64(op_val);
			#else
				op_val = __builtin_bswap64(op_val);
			#endif
			
			return access_as<T&>(&op_val);
		}
	}

	namespace detail
	{
		template<std::endian Order, Scalar T>
		T convert_endian(T value)
		{
			if constexpr ( std::endian::native != Order )
				return StdExt::swap_endianness(value);
			else
				return value;
		}
	}

	/**
	 * @brief
	 *  Converts from the native byte order to big endian.
	 */
	template<Scalar T>
	static T to_big_endian(T value)
	{
		return detail::convert_endian<std::endian::big>(value);
	}

	/**
	 * @brief
	 *  Converts from the native byte order to little endian.
	 */
	template<Scalar T>
	static T to_little_endian(T value)
	{
		return detail::convert_endian<std::endian::little>(value);
	}

	/**
	 * @brief
	 *  Converts from big endian to the native byte order.
	 */
	template<Scalar T>
	static T from_big_endian(T value)
	{
		return detail::convert_endian<std::endian::big>(value);
	}

	/**
	 * @brief
	 *  Converts from little endian the native byte order.
	 */
	template<Scalar T>
	static T from_little_endian(T value)
	{
		return detail::convert_endian<std::endian::little>(value);
	}
}

#endif // !_STD_EXT_MEMORY_ENDIANESS_H_