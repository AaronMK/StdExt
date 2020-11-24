#ifndef _STD_EXT_EXCEPTIONS_H_
#define _STD_EXT_EXCEPTIONS_H_

#include "Concepts.h"

#include <stdexcept>
#include <string>

namespace StdExt
{
	/**
	 * @brief
	 *  A scafolding class that can be used to attach file and line information
	 *  to any exception type.  Because this will derive from _base_ex_, it will
	 *  be picked up by any exception logic aware of that exception type.
	 *
	 * @todo
	 *  Make this use std::source_location when support is added.
	 */
	template<SubclassOf<std::exception> base_ex>
	class Exception : public base_ex
	{
		std::string Filename;
		size_t LineNumber;
	public:
		Exception(const char* filename, size_t line_number)
			requires std::is_default_constructible_v<base_ex>
			: Filename(filename), LineNumber(line_number)
		{
		}
		
		Exception(const std::string& what_arg, const char* filename, size_t line_number)
			: base_ex(what_arg), Filename(filename), LineNumber(line_number)
		{
		}
		
		Exception(const char* what_arg, const char* filename, size_t line_number)
			: base_ex(what_arg), Filename(filename), LineNumber(line_number)
		{
		}

		size_t line() const
		{
			return LineNumber;
		}

		const std::string& filename() const
		{
			return Filename;
		}
	};

	/**
	 * @brief
	 *  Throws an excpetion of _exception_t_ with the _filename_ and _line_number_
	 *  information attached.
	 */
	template<SubclassOf<std::exception> exception_t>
		requires std::is_default_constructible_v<exception_t>
	static void throw_exception(const char* filename, size_t line_number)
	{
		throw Exception<exception_t>(filename, line_number);
	}

	template<SubclassOf<std::exception> exception_t>
	static void throw_exception(const std::string& what_arg, const char* filename, size_t line_number)
	{
		throw Exception<exception_t>(what_arg, filename, line_number);
	}

	template<SubclassOf<std::exception> exception_t>
	static void throw_exception(const char* what_arg, const char* filename, size_t line_number)
	{
		throw Exception<exception_t>(what_arg, filename, line_number);
	}

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