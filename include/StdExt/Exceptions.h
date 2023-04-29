#ifndef _STD_EXT_EXCEPTIONS_H_
#define _STD_EXT_EXCEPTIONS_H_

#include "Concepts.h"
#include "Platform.h"

#ifndef STD_EXT_APPLE
#	include <source_location>
#endif

#include <stdexcept>
#include <string>

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

	class null_pointer : public std::logic_error
	{
	public:
		null_pointer();
		null_pointer(const std::string& what_arg);
		null_pointer(const char* what_arg);
	};

	class object_destroyed : public std::runtime_error
	{
	public:
		object_destroyed();
		object_destroyed(const std::string& what_arg);
		object_destroyed(const char* what_arg);
	};

	class time_out : public std::runtime_error
	{
	public:
		time_out();
		time_out(const std::string& what_arg);
		time_out(const char* what_arg);
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

	//////////////////////////////

	inline object_destroyed::object_destroyed()
		: std::runtime_error("Attempting to dereference as null pointer.")
	{
	}

	inline object_destroyed::object_destroyed(const std::string& what_arg)
		: std::runtime_error(what_arg)
	{
	}

	inline object_destroyed::object_destroyed(const char* what_arg)
		: std::runtime_error(what_arg)
	{
	}

	//////////////////////////////

	inline time_out::time_out()
		: std::runtime_error("Attempting to dereference as null pointer.")
	{
	}

	inline time_out::time_out(const std::string& what_arg)
		: std::runtime_error(what_arg)
	{
	}

	inline time_out::time_out(const char* what_arg)
		: std::runtime_error(what_arg)
	{
	}
}

#endif // _STD_EXT_EXCEPTIONS_H_