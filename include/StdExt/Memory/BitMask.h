#ifndef _STD_EXT_MEMORY_BIT_MASK_H_
#define _STD_EXT_MEMORY_BIT_MASK_H_

#include "../Concepts.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace StdExt
{
	/**
	 * @brief
	 *  Creates a bitmask of type T where the least significant bit_count bits are ones.
	 */
	template<Unsigned T>
	constexpr T postfixMask(uint8_t bit_count)
	{
		assert(bit_count <= sizeof(T) * 8);

		if (bit_count == 0)
			return T{0};

		if (bit_count >= sizeof(T) * 8)
			return std::numeric_limits<T>::max();

		return ( T{1} << bit_count ) - 1;
	}

	/**
	 * @brief
	 *  Creates a bitmask of type T where the most significant bit_count bits are ones.
	 */
	template<Unsigned T>
	constexpr T prefixMask(uint8_t bit_count)
	{
		assert(bit_count <= sizeof(T) * 8);
		return postfixMask<T>(sizeof(T) * 8 - bit_count) ^ std::numeric_limits<T>::max();
	}

	/**
	 * @brief
	 *  Creates a mask where bits in positions [low_bit, high_bit] (inclusive,
	 *  0-indexed from the least significant bit) are ones.
	 *
	 * @param high_bit
	 *  The highest bit position to set. Must be greater than or equal to low_bit.
	 *
	 * @param low_bit
	 *  The lowest bit position to set. Must be less than or equal to high_bit.
	 */
	template<Unsigned T>
	constexpr T bitMask(uint8_t high_bit, uint8_t low_bit)
	{
		assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		return postfixMask<T>(high_bit + 1) ^ postfixMask<T>(low_bit);
	}

	/**
	 * @brief
	 *  Returns value with only the bits in positions [low_bit, high_bit]
	 *  (inclusive, 0-indexed from the least significant bit) preserved; all others are zero.
	 *
	 * @param value
	 *  The value from which to preserve bits.
	 *
	 * @param high_bit
	 *  The highest bit position to preserve. Must be greater than or equal to low_bit.
	 *
	 * @param low_bit
	 *  The lowest bit position to preserve. Must be less than or equal to high_bit.
	 */
	template<Unsigned T>
	constexpr T maskBits(T value, uint8_t high_bit, uint8_t low_bit)
	{
		assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		return (value & bitMask<T>(high_bit, low_bit));
	}

	/**
	 * @brief
	 *  Returns value with only the bits in positions [low_bit, high_bit]
	 *  (inclusive, 0-indexed from the least significant bit) preserved; all others are zero.
	 *
	 * @details
	 *  This templated version can allow more compiler optimizations.
	 *
	 * @tparam high_bit
	 *  The highest bit position to preserve. Must be greater than or equal to low_bit.
	 *
	 * @tparam low_bit
	 *  The lowest bit position to preserve. Must be less than or equal to high_bit.
	 *
	 * @param value
	 *  The value from which to preserve bits.
	 */
	template<uint8_t high_bit, uint8_t low_bit, Unsigned T>
	constexpr T maskBits(T value)
	{
		static_assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		return (value & bitMask<T>(high_bit, low_bit));
	}

	/**
	 * @brief
	 *  Extracts the bits in positions [low_bit, high_bit] (inclusive,
	 *  0-indexed from the least significant bit) and returns their value right-shifted
	 *  to start at bit 0.
	 *
	 * @param value
	 *  The value from which to extract bits.
	 *
	 * @param high_bit
	 *  The highest bit position to extract. Must be greater than or equal to low_bit.
	 *
	 * @param low_bit
	 *  The lowest bit position to extract. Must be less than or equal to high_bit.
	 */
	template<Unsigned T>
	constexpr T maskValue(T value, uint8_t high_bit, uint8_t low_bit)
	{
		assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		return (value & bitMask<T>(high_bit, low_bit)) >> low_bit;
	}

	/**
	 * @brief
	 *  Extracts the bits in positions [low_bit, high_bit] (inclusive,
	 *  0-indexed from the least significant bit) and returns their value right-shifted
	 *  to start at bit 0.
	 *
	 * @details
	 *  This templated version can allow more compiler optimizations.
	 *
	 * @tparam high_bit
	 *  The highest bit position to extract. Must be greater than or equal to low_bit.
	 *
	 * @tparam low_bit
	 *  The lowest bit position to extract. Must be less than or equal to high_bit.
	 *
	 * @param value
	 *  The value from which to extract bits.
	 */
	template<uint8_t high_bit, uint8_t low_bit, Unsigned T>
	constexpr T maskValue(T value)
	{
		static_assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		return (value & bitMask<T>(high_bit, low_bit)) >> low_bit;
	}

	/**
	 * @brief
	 *  Returns value with the bits in positions [low_bit, high_bit] (inclusive,
	 *  0-indexed from the least significant bit) set to mask_value, which is treated as starting
	 *  at bit 0.
	 *
	 * @param value
	 *  The value into which mask_value will be inserted.
	 *
	 * @param mask_value
	 *  The value to insert, starting at bit 0. Must fit within high_bit - low_bit + 1 bits.
	 *
	 * @param high_bit
	 *  The highest bit position to set. Must be greater than or equal to low_bit.
	 *
	 * @param low_bit
	 *  The lowest bit position to set. Must be less than or equal to high_bit.
	 */
	template<Unsigned T>
	constexpr T setMaskValue(T value, T mask_value, uint8_t high_bit, uint8_t low_bit)
	{
		assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		assert(mask_value <= postfixMask<T>(high_bit - low_bit + 1));
		return (value & ~bitMask<T>(high_bit, low_bit)) | (mask_value << low_bit);
	}

	/**
	 * @brief
	 *  Returns value with the bits in positions [low_bit, high_bit] (inclusive,
	 *  0-indexed from the least significant bit) set to mask_value, which is treated as starting
	 *  at bit 0.
	 *
	 * @details
	 *  This templated version can allow more compiler optimizations.
	 *
	 * @tparam high_bit
	 *  The highest bit position to set. Must be greater than or equal to low_bit.
	 *
	 * @tparam low_bit
	 *  The lowest bit position to set. Must be less than or equal to high_bit.
	 *
	 * @param value
	 *  The value into which mask_value will be inserted.
	 *
	 * @param mask_value
	 *  The value to insert, starting at bit 0. Must fit within high_bit - low_bit + 1 bits.
	 */
	template<uint8_t high_bit, uint8_t low_bit, Unsigned T>
	constexpr T setMaskValue(T value, T mask_value)
	{
		static_assert(high_bit >= low_bit && high_bit < sizeof(T) * 8);
		assert(mask_value <= postfixMask<T>(high_bit - low_bit + 1));
		return (value & ~bitMask<T>(high_bit, low_bit)) | (mask_value << low_bit);
	}
}

#endif // !_STD_EXT_MEMORY_BIT_MASK_H_
