#include <StdExt/Serialize/Exceptions.h>

namespace StdExt::Serialize
{
	FormatException::FormatException()
		: std::runtime_error("Invalid data detected during serialization.")
	{
	}

	FormatException::FormatException(const char* message)
		: std::runtime_error(message)
	{
	}

	FormatException::FormatException(const std::string& strMsg)
		: std::runtime_error(strMsg)
	{
	}

	//////////////////////////////////////

	InvalidOperation::InvalidOperation()
		: std::logic_error("An invalid operation was attempted.")
	{

	}

	InvalidOperation::InvalidOperation(const char* message)
		: std::logic_error(message)
	{
	}

	InvalidOperation::InvalidOperation(const std::string& message)
		: std::logic_error(message)
	{
	}

	//////////////////////////////////////

	OutOfBounds::OutOfBounds()
		: std::out_of_range("Reading, writing, or seeking outside the boundaries of the stream was attempted.")
	{

	}

	OutOfBounds::OutOfBounds(const char* message)
		: std::out_of_range(message)
	{
	}

	OutOfBounds::OutOfBounds(const std::string& message)
		: std::out_of_range(message)
	{
	}

	namespace XML
	{
		ElementNotFound::ElementNotFound()
			: FormatException("Requested element was not found.")
		{
		}

		ElementNotFound::ElementNotFound(const std::string& elmName)
			: FormatException("Requested element '" + elmName + "'was not found.")
		{
		}
	}
}