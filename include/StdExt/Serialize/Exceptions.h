#ifndef _STD_EXT_SERIALIZE_EXCEPTIONS_H_
#define _STD_EXT_SERIALIZE_EXCEPTIONS_H_

#include "../String.h"

#include <stdexcept>

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
		FormatException(const StdExt::String& strMsg);
	};

	///////////////////////////////////

	class InvalidOperation : public std::logic_error
	{
	public:
		InvalidOperation();
		InvalidOperation(const char* message);
		InvalidOperation(const std::string& message);
		InvalidOperation(const StdExt::String& message);
	};

	///////////////////////////////////

	class OutOfBounds : public std::out_of_range
	{
	public:
		OutOfBounds();
		OutOfBounds(const char* message);
		OutOfBounds(const std::string& message);
	};

	/////////////////////////////////

	inline FormatException::FormatException()
		: std::runtime_error("Invalid data detected during serialization.")
	{
	}

	inline FormatException::FormatException(const char* message)
		: std::runtime_error(message)
	{
	}

	inline FormatException::FormatException(const std::string& strMsg)
		: std::runtime_error(strMsg)
	{
	}

	inline FormatException::FormatException(const StdExt::String& strMsg)
		: std::runtime_error(strMsg.getNullTerminated().data())
	{
	}

	//////////////////////////////////////

	inline InvalidOperation::InvalidOperation()
		: std::logic_error("An invalid operation was attempted.")
	{

	}

	inline InvalidOperation::InvalidOperation(const char* message)
		: std::logic_error(message)
	{
	}

	inline InvalidOperation::InvalidOperation(const std::string& message)
		: std::logic_error(message)
	{
	}

	inline InvalidOperation::InvalidOperation(const StdExt::String& message)
		: std::logic_error(message.getNullTerminated().data())
	{
	}

	//////////////////////////////////////

	inline OutOfBounds::OutOfBounds()
		: std::out_of_range("Reading, writing, or seeking outside the boundaries of the stream was attempted.")
	{

	}

	inline OutOfBounds::OutOfBounds(const char* message)
		: std::out_of_range(message)
	{
	}

	inline OutOfBounds::OutOfBounds(const std::string& message)
		: std::out_of_range(message)
	{
	}

	namespace XML
	{
		class ElementNotFound : public FormatException
		{
		public:
			ElementNotFound();
			ElementNotFound(const StdExt::String& elmName);
		};

		//////////////////////////////////////

		inline ElementNotFound::ElementNotFound()
			: FormatException("Requested element was not found.")
		{
		}

		inline ElementNotFound::ElementNotFound(const StdExt::String& elmName)
			: FormatException("Requested element '" + elmName + "'was not found.")
		{
		}
	}
}

#endif // _STD_EXT_SERIALIZE_EXCEPTIONS_H_
