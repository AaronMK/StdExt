#include <StdExt/Number.h>

#include <string>

using namespace std;

namespace StdExt
{
	Number Number::parse(std::string_view str)
	{
		double parsedDouble = stod(str.data());
		
		if (0.0 != fmod(parsedDouble, 1.0))
		{
			return Number(parsedDouble);
		}
		else if (parsedDouble >= 0.0)
		{
			try
			{
				return Number(stoull(str.data()));
			}
			catch (out_of_range)
			{
				return Number(parsedDouble);
			}
		}
		else
		{
			try
			{
				return Number(stoll(str.data()));
			}
			catch (out_of_range)
			{
				return Number(parsedDouble);
			}
		}
	}

	Number::Number()
	{
		mValue.emplace<int64_t>(0);
	}

	Number::Number(uint64_t value)
	{
		mValue.emplace<uint64_t>(value);
	}

	Number::Number(int64_t value)
	{
		mValue.emplace<int64_t>(value);
	}

	Number::Number(double_t value)
	{
		mValue.emplace<double_t>(value);
	}

	Number::operator uint8_t() const
	{
		return value<uint8_t>();
	}

	Number::operator uint16_t() const
	{
		return value<uint16_t>();
	}

	Number::operator uint32_t() const
	{
		return value<uint32_t>();
	}

	Number::operator uint64_t() const
	{
		return value<uint64_t>();
	}

	Number::operator int8_t() const
	{
		return value<int8_t>();
	}

	Number::operator int16_t() const
	{
		return value<int16_t>();
	}

	Number::operator int32_t() const
	{
		return value<int32_t>();
	}

	Number::operator int64_t() const
	{
		return value<int64_t>();
	}
}