#ifndef _STD_EXT_EXCEPTIONS_H_
#define _STD_EXT_EXCEPTIONS_H_

#include "Concepts.h"

#include <source_location>
#include <stdexcept>
#include <string>

namespace StdExt
{
	/**
	 * @brief
	 *  A scafolding class that attaches source_location information
	 *  to any exception type.  Because this will derive from _base_ex_, it will
	 *  be picked up by any exception logic aware of that exception type.
	 */
	template<SubclassOf<std::exception> base_ex>
	class Exception : public base_ex
	{
		std::source_location mSourceLocation;

	public:
		Exception(std::source_location src_loc = std::source_location::current())
			: mSourceLocation(src_loc)
		{
		}

		Exception(const char* what, std::source_location src_loc = std::source_location::current())
			: base_ex(what), mSourceLocation(src_loc)
		{
		}

		std::source_location sourceLocation() const
		{
			return mSourceLocation;
		}
	};

	///////////////////////////////

	class invalid_operation : public std::logic_error
	{
	public:
		invalid_operation(const std::string& what_arg);
		invalid_operation(const char* what_arg);
	};

	class filesystem_error : public std::runtime_error
	{
	public:
		filesystem_error(const std::string& what_arg);
		filesystem_error(const char* what_arg);
	};

	class not_implemented : public std::logic_error
	{
	public:
		not_implemented();
		not_implemented(const std::string& what_arg);
		not_implemented(const char* what_arg);
	};

	class null_pointer : public std::logic_error
	{
	public:
		null_pointer();
		null_pointer(const std::string& what_arg);
		null_pointer(const char* what_arg);
	};

	//////////////////////////////

	inline invalid_operation::invalid_operation(const std::string& what_arg)
		: std::logic_error(what_arg)
	{
	}


	inline invalid_operation::invalid_operation(const char* what_arg)
		: std::logic_error(what_arg)
	{
	}

	//////////////////////////////

	inline filesystem_error::filesystem_error(const std::string& what_arg)
		: std::runtime_error(what_arg)
	{
	}


	inline filesystem_error::filesystem_error(const char* what_arg)
		: std::runtime_error(what_arg)
	{
	}

	//////////////////////////////

	inline not_implemented::not_implemented()
		: std::logic_error("Functionality not yet implemented.")
	{
	}

	inline not_implemented::not_implemented(const std::string& what_arg)
		: std::logic_error(what_arg)
	{
	}

	inline not_implemented::not_implemented(const char* what_arg)
		: std::logic_error(what_arg)
	{
	}

	//////////////////////////////

	inline null_pointer::null_pointer()
		: std::logic_error("Attempting to dereference as null pointer.")
	{
	}

	inline null_pointer::null_pointer(const std::string& what_arg)
		: std::logic_error(what_arg)
	{
	}

	inline null_pointer::null_pointer(const char* what_arg)
		: std::logic_error(what_arg)
	{
	}
}

#endif // _STD_EXT_EXCEPTIONS_H_