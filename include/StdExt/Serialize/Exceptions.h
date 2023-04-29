#ifndef _STD_EXT_SERIALIZE_EXCEPTIONS_H_
#define _STD_EXT_SERIALIZE_EXCEPTIONS_H_

#include <stdexcept>
#include <string>

namespace StdExt::Serialize
{
	class FormatException : public std::runtime_error
	{
	public:
		template<typename T>
		static FormatException ForType()
		{
			return FormatException(std::string("Invalid data when reading ") + typeid(T).name() + ".");
		}

		FormatException();
		FormatException(const char* message);
		FormatException(const std::string& strMsg);
	};

	///////////////////////////////////

	class InvalidOperation : public std::logic_error
	{
	public:
		InvalidOperation();
		InvalidOperation(const char* message);
		InvalidOperation(const std::string& message);
	};

	///////////////////////////////////

	class OutOfBounds : public std::out_of_range
	{
	public:
		OutOfBounds();
		OutOfBounds(const char* message);
		OutOfBounds(const std::string& message);
	};

	namespace XML
	{
		class ElementNotFound : public FormatException
		{
		public:
			ElementNotFound();
			ElementNotFound(const std::string& message);
		};
	}
}

#endif // _STD_EXT_SERIALIZE_EXCEPTIONS_H_
