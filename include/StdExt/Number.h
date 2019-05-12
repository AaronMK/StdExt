#ifndef _STD_EXT_NUMBER_H_
#define _STD_EXT_NUMBER_H_

#include "StdExt.h"
#include "String.h"

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
	using float32_t = float;
	using float64_t = double;

	constexpr char* RangeMessage = "Numeric conversion out of range.";

	template<typename T>
	static constexpr T MinVal()
	{
		static_assert(std::is_arithmetic_v<T>, "T must be numeric.");
		return std::numeric_limits<T>::min();
	};

	template<typename T>
	static constexpr T MaxVal()
	{
		static_assert(std::is_arithmetic_v<T>, "T must be numeric.");
		return std::numeric_limits<T>::max();
	};

	/**
	 * @brief
	 *	Container that can contain a numeric within the full range
	 *  of all numeric primitive types.  It also performs bound checked
	 *  conversion operators.
	 */
	class STD_EXT_EXPORT Number
	{
	private:
		std::variant<double_t, int64_t, uint64_t> mValue;

	public:

		/**
		 * @brief
		 *  A function that will perform checked conversion between numeric types, throwing
		 *  a range_error exception if the conversion will cause an overflow or underflow.
		 *  Conversions from floating point to interger types are rounded.
		 */
		template<typename result_t, typename value_t>
		static result_t convert(value_t value)
		{
			static_assert(std::is_arithmetic_v<result_t>, "result_t must be numeric.");
			static_assert(std::is_arithmetic_v<value_t>, "value_t must be numeric.");

			if constexpr (std::is_same_v<result_t, value_t>)
			{
				return value;
			}
			else if constexpr (std::is_signed_v<value_t> == std::is_signed_v<result_t>)
			{
				if (MinVal<result_t>() <= value && value <= MaxVal<result_t>())
					return (result_t)value;
				else
					throw std::range_error(RangeMessage);
			}
			else if constexpr (std::is_unsigned_v<result_t>)
			{
				// value_t must be signed
				if (value < 0)
					throw std::range_error(RangeMessage);

				if constexpr (std::is_integral_v<value_t>)
				{
					if ((uint64_t)value > MaxVal<result_t>())
						throw std::range_error(RangeMessage);
					else
						return (result_t)value;
				}
				else
				{
					if ((double_t)value > MaxVal<result_t>())
						throw std::range_error(RangeMessage);
					return
						(result_t)value;
				}
			}
			else
			{
				// value_t is unsigned, result_t is signed
				if constexpr (std::is_integral_v<result_t>)
				{

					int64_t intValue = (int64_t)value;

					// if the conversion oveflows on most precise int type
					// we know it will overflow anything
					if (intValue < 0 || intValue > MaxVal<result_t>())
						throw std::range_error(RangeMessage);
					else
						return (result_t)value;
				}
				else
				{
					double_t doubleVal = (double_t)value;

					if (MinVal<result_t>() > doubleVal || doubleVal > MaxVal<result_t>())
						throw std::range_error(RangeMessage);
					else
						return (result_t)value;
				}
			}
		};

		Number(const Number&) = default;
		Number& operator=(const Number&) = default;
		
		Number();

		Number(int8_t value);
		Number(int16_t value);
		Number(int32_t value);
		Number(int64_t value);

		Number(uint8_t value);
		Number(uint16_t value);
		Number(uint32_t value);
		Number(uint64_t value);

		Number(float32_t value);
		Number(float64_t value);

		template<typename T>
		T value() const
		{
			static_assert(std::is_arithmetic_v<T>, "T must be numeric.");

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

		/**
		 * @brief
		 *  String conversion that provides output suitable for most applications.
		 */
		String toString() const;

		/**
		 * @brief
		 *  Parses the string into a number.
		 */
		static Number parse(std::string_view str);
	};
}

#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // !_STD_EXT_NUMBER_H_