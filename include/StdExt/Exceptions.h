#ifndef _STD_EXT_EXCEPTIONS_H_
#define _STD_EXT_EXCEPTIONS_H_

#include "StdExt.h"

#include <stdexcept>

namespace StdExt
{
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
}

#endif // _STD_EXT_EXCEPTIONS_H_