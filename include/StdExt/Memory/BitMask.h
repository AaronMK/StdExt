#ifndef _STD_EXT_MEMORY_BIT_MASK_H_
#define _STD_EXT_MEMORY_BIT_MASK_H_

#include "../Concepts.h"

namespace StdExt
{
	/**
	 * @brief
	 *  Creates a bitmask of type T where the right-most bit_count bits are ones.
	 */
	template<Unsigned T>
	static constexpr T postfixMask(uint8_t bit_count)
	{
		if ( bit_count == sizeof(T) * 8)
			return std::numeric_limits<T>::max();

		return ( T{1} << bit_count ) - 1;
	}
	
	/**
	 * @brief
	 *  Creates a bitmask of type T where the left-most bit_count bits are ones.
	 */
	template<Unsigned T>
	static constexpr T prefixMask(uint8_t bit_count)
	{
		return postfixMask<T>(sizeof(T) * 8 - bit_count) ^ std::numeric_limits<T>::max();
	}

	/**
	 * @brief
	 *  Creates a mask to isolate bits to the left of high-bit and bits
	 *  to the right of low bits.  (0-based indexing is used.)
	 */
	template<Unsigned T>
	static constexpr T bitMask(uint8_t high_bit, uint8_t low_bit)
	{
		return postfixMask<T>(high_bit + 1) ^ postfixMask<T>(low_bit);
	}

	/**
	 * @brief
	 *  Isolate bits to the left of high-bit and bits
	 *  to the right of low bits. (0-based indexing is used.)
	 */
	template<Unsigned T>
	static constexpr T maskBits(T value, uint8_t high_bit, uint8_t low_bit)
	{
		return (value & bitMask<T>(high_bit, low_bit));
	}

	/**
	 * @brief
	 *  Isolate bits to the left of high-bit and bits
	 *  to the right of low bits.  (0-based indexing is used.)
	 * 
	 * @details
	 *  This templated version can allow more compiler optimizations.
	 */
	template<uint8_t high_bit, uint8_t low_bit, Unsigned T>
	static constexpr T maskBits(T value)
	{
		return (value & bitMask<T>(high_bit, low_bit));
	}

	/**
	 * @brief
	 *  Isolate bits to the left of high-bit and bits
	 *  to the right of low bits, and gets their value as
	 *  if they were the right-most bits.
	 *  (0-based indexing is used.)
	 */
	template<Unsigned T>
	static constexpr T maskValue(T value, uint8_t high_bit, uint8_t low_bit)
	{
		return (value & bitMask<T>(high_bit, low_bit)) >> std::min(high_bit, low_bit);
	}

	/**
	 * @brief
	 *  Isolate bits to the left of high-bit and bits
	 *  to the right of low bits, and gets their value as
	 *  if they were the right-most bits.
	 *  (0-based indexing is used.)
	 * 
	 * @details
	 *  This templated version can allow more compiler optimizations.
	 */
	template<uint8_t high_bit, uint8_t low_bit, Unsigned T>
	static constexpr T maskValue(T value)
	{
		return (value & bitMask<T>(high_bit, low_bit)) >> std::min(high_bit, low_bit);
	}
}

#endif // !_STD_EXT_MEMORY_BIT_MASK_H_