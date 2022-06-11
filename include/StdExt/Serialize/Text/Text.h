#ifndef _STD_EXT_SERIALIZE_TEXT_TEXT_H_
#define _STD_EXT_SERIALIZE_TEXT_TEXT_H_

#include "../../String.h"
#include "../../Number.h"

namespace StdExt::Serialize::Text
{
	template<typename T>
	void read(const StdExt::String& string, T* out) = delete;

	template<typename T>
	StdExt::String write(const T& val) = delete;

	template<StdExt::Arithmetic T>
	void read(const StdExt::String& string, T* out)
	{
		*out = Number::parse(string).value<T>();
	}

	template<StdExt::Arithmetic T>
	StdExt::String write(const T& val)
	{
		return Number(val).toString();
	}

	template<>
	STD_EXT_EXPORT void read<bool>(const StdExt::String& string, bool* out);

	template<>
	STD_EXT_EXPORT StdExt::String write<bool>(const bool& val);

	template<>
	STD_EXT_EXPORT void read<StdExt::Number>(const StdExt::String& string, StdExt::Number* out);

	template<>
	STD_EXT_EXPORT StdExt::String write<StdExt::Number>(const StdExt::Number& val);

	template<>
	STD_EXT_EXPORT void read<bool>(const StdExt::String& string, bool* out);

	template<>
	STD_EXT_EXPORT StdExt::String write<bool>(const bool& val);

	template<>
	STD_EXT_EXPORT void read<StdExt::String>(const StdExt::String& string, StdExt::String* out);

	template<>
	STD_EXT_EXPORT StdExt::String write<StdExt::String>(const StdExt::String& val);

	template<Defaultable T>
	T read(const StdExt::String& val)
	{
		T out;
		read<T>(val, &out);

		return out;
	}
}

#endif // !_STD_EXT_SERIALIZE_TEXT_TEXT_H_
