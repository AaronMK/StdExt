#ifndef _STD_EXT_NUMBER_H_
#define _STD_EXT_NUMBER_H_

#include "Concepts.h"
#include "Exceptions.h"
#include "String.h"
#include "Type.h"
#include "Memory/BitMask.h"

#include <type_traits>
#include <stdexcept>
#include <cstdint>
#include <variant>
#include <limits>
#include <cmath>

#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 4251 )
#endif

namespace StdExt
{
	static const char* RangeMessage = "Numeric conversion out of range.";

	/**
	 * @brief
	 *  Container that can contain a numeric value within the full range
	 *  of all numeric primitive types.  It also performs bound checked
	 *  conversions.
	 */
	class STD_EXT_EXPORT Number
	{
	private:
		std::variant<float64_t, int64_t, uint64_t> mValue;

		/**
		 * @brief
		 *  Gets the minimum floating point value that can be stored
		 *  in <i>IntType</i>.
		 */
		template<Integral IntType, FloatingPoint FloatType>
		static constexpr FloatType MinFloatValue()
		{
			if constexpr (Unsigned<IntType>)
				return FloatType(0.0);
			else
				return static_cast<FloatType>(std::numeric_limits<IntType>::lowest());
		}

		/**
		 * @brief
		 *  Gets the maximum floating point value that can be stored
		 *  in <i>IntType</i>.
		 */
		template<Integral IntType, FloatingPoint FloatType>
		static constexpr FloatType MaxFloatValue()
		{
			// Number of significant bits the float type can represent exactly.
			constexpr int sig_bits = std::numeric_limits<FloatType>::digits;

			// Value bits in IntType (excludes the sign bit for signed types).
			constexpr int val_bits = std::is_signed_v<IntType>
				? static_cast<int>(sizeof(IntType)) * 8 - 1
				: static_cast<int>(sizeof(IntType)) * 8;

			if constexpr (val_bits <= sig_bits)
			{
				// IntType::max() is exactly representable in FloatType.
				return static_cast<FloatType>(std::numeric_limits<IntType>::max());
			}
			else
			{
				// IntType::max() = 2^val_bits - 1, which rounds up when cast to FloatType.
				// The largest representable FloatType that stays within range is:
				//   2^val_bits - 2^(val_bits - sig_bits)
				FloatType high = FloatType(1.0);
				for (int i = 0; i < val_bits; i++)
					high *= FloatType(2.0);

				FloatType low = FloatType(1.0);
				for (int i = 0; i < val_bits - sig_bits; i++)
					low *= FloatType(2.0);

				return high - low;
			}
		}

	public:

		/**
		 * @brief
		 *  A function that will perform checked conversion between numeric types, throwing
		 *  a range_error exception if the conversion will cause an overflow or underflow.
		 *  Conversions from floating point to integer types are truncated toward zero in
		 *  the same way an explicit cast would.
		 */
		template<Arithmetic result_t, Arithmetic value_t>
		static constexpr result_t convert(value_t value)
		{
			if constexpr (std::is_same_v<result_t, value_t>)
			{
				// Same type: no conversion needed.
				return value;
			}
			else if constexpr (!FloatingPoint<value_t> && FloatingPoint<result_t>)
			{
				// Integer -> Float: all integer values fit within float/double range.
				return static_cast<result_t>(value);
			}
			else if constexpr (
					(FloatingPoint<value_t> && FloatingPoint<result_t>) ||
					(SignedIntegral<value_t> && SignedIntegral<result_t>)
				)
			{
				if constexpr (sizeof(value_t) < sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = std::numeric_limits<result_t>::max();
					constexpr value_t min_val = std::numeric_limits<result_t>::lowest();

					if (value < min_val || value > max_val)
						throw std::range_error(RangeMessage);

					return static_cast<result_t>(value);
				}
			}
			else if constexpr (FloatingPoint<value_t> && Integral<result_t>)
			{
				if (std::isnan(value))
					throw invalid_operation("Cannot convert not-a-number to an integer type.");

				constexpr value_t min_representable = MinFloatValue<result_t, value_t>();
				constexpr value_t max_representable = MaxFloatValue<result_t, value_t>();

				if (value < min_representable || value > max_representable)
					throw std::range_error(RangeMessage);

				return static_cast<result_t>(value);
			}
			else if constexpr (Unsigned<value_t> && Unsigned<result_t>)
			{
				if constexpr (sizeof(value_t) <= sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = static_cast<value_t>(std::numeric_limits<result_t>::max());
					
					if (value > max_val)
						throw std::range_error(RangeMessage);
					
					return static_cast<result_t>(value);
				}
			}
			else if constexpr (SignedIntegral<value_t> && Unsigned<result_t>)
			{
				if (value < 0)
					throw std::range_error(RangeMessage);

				if constexpr (sizeof(value_t) <= sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = static_cast<value_t>(std::numeric_limits<result_t>::max());

					if (value > max_val)
						throw std::range_error(RangeMessage);

					return static_cast<result_t>(value);
				}
			}
			else if constexpr (Unsigned<value_t> && SignedIntegral<result_t>)
			{
				if constexpr (sizeof(value_t) < sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = static_cast<value_t>(std::numeric_limits<result_t>::max());

					if (value > max_val)
						throw std::range_error(RangeMessage);

					return static_cast<result_t>(value);
				}
			}
			else
			{
				// Should never get here.
				throw std::logic_error("Unexpected combination of types for numeric conversion.");
			}
		};

		/**
		 * @brief
		 *  A function that performs a clamping conversion between numeric types.  If the value
		 *  is outside the range of result_t, it is clamped to the nearest representable value.
		 *  Conversions from floating point to integer types are truncated toward zero in
		 *  the same way an explicit cast would.  Attempting to convert a not-a-number floating
		 *  point value to an integer type throws invalid_operation.
		 */
		template<Arithmetic result_t, Arithmetic value_t>
		static constexpr result_t clampConvert(value_t value)
		{
			if constexpr (std::is_same_v<result_t, value_t>)
			{
				return value;
			}
			else if constexpr (!FloatingPoint<value_t> && FloatingPoint<result_t>)
			{
				// Integer -> Float: all integer values fit within float/double range.
				return static_cast<result_t>(value);
			}
			else if constexpr (
					(FloatingPoint<value_t> && FloatingPoint<result_t>) ||
					(SignedIntegral<value_t> && SignedIntegral<result_t>)
				)
			{
				if constexpr (sizeof(value_t) < sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = std::numeric_limits<result_t>::max();
					constexpr value_t min_val = std::numeric_limits<result_t>::lowest();

					if (value > max_val)
						return std::numeric_limits<result_t>::max();

					if (value < min_val)
						return std::numeric_limits<result_t>::lowest();

					return static_cast<result_t>(value);
				}
			}
			else if constexpr (FloatingPoint<value_t> && Integral<result_t>)
			{
				if (std::isnan(value))
					throw invalid_operation("Cannot convert not-a-number to an integer type.");

				constexpr value_t min_representable = MinFloatValue<result_t, value_t>();
				constexpr value_t max_representable = MaxFloatValue<result_t, value_t>();

				if (value > max_representable)
					return std::numeric_limits<result_t>::max();

				if (value < min_representable)
					return std::numeric_limits<result_t>::lowest();

				return static_cast<result_t>(value);
			}
			else if constexpr (Unsigned<value_t> && Unsigned<result_t>)
			{
				if constexpr (sizeof(value_t) <= sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = static_cast<value_t>(std::numeric_limits<result_t>::max());

					if (value > max_val)
						return std::numeric_limits<result_t>::max();

					return static_cast<result_t>(value);
				}
			}
			else if constexpr (SignedIntegral<value_t> && Unsigned<result_t>)
			{
				if (value < 0)
					return result_t(0);

				if constexpr (sizeof(value_t) <= sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = static_cast<value_t>(std::numeric_limits<result_t>::max());

					if (value > max_val)
						return std::numeric_limits<result_t>::max();

					return static_cast<result_t>(value);
				}
			}
			else if constexpr (Unsigned<value_t> && SignedIntegral<result_t>)
			{
				if constexpr (sizeof(value_t) < sizeof(result_t))
				{
					return static_cast<result_t>(value);
				}
				else
				{
					constexpr value_t max_val = static_cast<value_t>(std::numeric_limits<result_t>::max());

					if (value > max_val)
						return std::numeric_limits<result_t>::max();

					return static_cast<result_t>(value);
				}
			}
			else
			{
				throw std::logic_error("Unexpected combination of types for numeric conversion.");
			}
		};

		Number(const Number&) = default;
		Number& operator=(const Number&) = default;
		
		/**
		 * @brief
		 *  Creates a number object with a value of zero.
		 */
		Number();

		template<SignedIntegral T>
		Number(T value)
		{
			mValue.emplace<int64_t>(value);
		}

		template<Unsigned T>
		Number(T value)
		{
			mValue.emplace<uint64_t>(value);
		}

		template<FloatingPoint T>
		Number(T value)
		{
			mValue.emplace<float64_t>(value);
		}

		template<Arithmetic T>
		T value() const
		{
			if (std::holds_alternative<int64_t>(mValue))
				return convert<T>(std::get<int64_t>(mValue));
			else if (std::holds_alternative<uint64_t>(mValue))
				return convert<T>(std::get<uint64_t>(mValue));
			else if (std::holds_alternative<double_t>(mValue))
				return convert<T>(std::get<double_t>(mValue));
			else
				throw std::logic_error("No numeric value is held.");
		}

		operator uint8_t() const;
		operator uint16_t() const;
		operator uint32_t() const;
		operator uint64_t() const;

		operator int8_t() const;
		operator int16_t() const;
		operator int32_t() const;
		operator int64_t() const;

		operator float32_t() const;
		operator float64_t() const;

		String toString() const;

		/**
		 * @brief
		 *  Parses the string into a number.
		 */
		static Number parse(const String& str);

		/**
		 * @brief
		 *  Gets the underlying storage type used for the current value as
		 *  std::type_info.
		 */
		const std::type_info& storedAsInfo() const noexcept;

		/**
		 * @brief
		 *  Gets the underlying storage type used for the current value as
		 *  std::type_index.
		 */
		std::type_index storedAsIndex() const noexcept;
	};
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // !_STD_EXT_NUMBER_H_