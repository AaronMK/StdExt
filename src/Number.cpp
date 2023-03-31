#include <StdExt/Number.h>
#include <StdExt/Buffer.h>

#include <array>
#include <cstdio>
#include <string>

using namespace std;

namespace StdExt
{

	Number::Number()
	{
		mValue.emplace<int64_t>(0);
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

	Number::operator float32_t() const
	{
		return value<float32_t>();
	}

	Number::operator float64_t() const
	{
		return value<float64_t>();
	}

	String Number::toString() const
	{
		std::wstring strOut;
		
		if (std::holds_alternative<int64_t>(mValue))
			strOut = std::to_wstring(std::get<int64_t>(mValue));
		else if (std::holds_alternative<uint64_t>(mValue))
			strOut = std::to_wstring(std::get<uint64_t>(mValue));
		else if (std::holds_alternative<double_t>(mValue))
			strOut = std::to_wstring(std::get<double_t>(mValue));
		else
			throw invalid_operation("Unknown internal numeric type.");

		return convertString<char8_t>(strOut);
	}

	Number Number::parse(const String& str)
	{
		std::wstring wstr = convertString<wchar_t>(str).toStdString();

		double parsedDouble = stod(wstr);

		if (0.0 != fmod(parsedDouble, 1.0))
		{
			return Number(parsedDouble);
		}
		else if (parsedDouble >= 0.0)
		{
			try
			{
				return Number(stoull(wstr.data()));
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
				return Number(stoll(wstr.data()));
			}
			catch (out_of_range)
			{
				return Number(parsedDouble);
			}
		}
	}

	const type_info& Number::storedAsInfo() const noexcept
	{
		if (std::holds_alternative<int64_t>(mValue))
			return typeid(int64_t);
		else if (std::holds_alternative<uint64_t>(mValue))
			return typeid(uint64_t);
		else if (std::holds_alternative<double_t>(mValue))
			return typeid(double_t);
		else
			return typeid(void);
	}

	type_index Number::storedAsIndex() const noexcept
	{
		return type_index(storedAsInfo());
	}
}