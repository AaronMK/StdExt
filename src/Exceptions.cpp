#include <StdExt/Exceptions.h>

namespace StdExt
{
	invalid_operation::invalid_operation(const std::string& what_arg)
		: std::logic_error(what_arg)
	{
	}


	invalid_operation::invalid_operation(const char* what_arg)
		: std::logic_error(what_arg)
	{
	}

	//////////////////////////////

	not_implemented::not_implemented()
		: std::logic_error("Functionality not yet implemented.")
	{
	}

	not_implemented::not_implemented(const std::string& what_arg)
		: std::logic_error(what_arg)
	{
	}

	not_implemented::not_implemented(const char* what_arg)
		: std::logic_error(what_arg)
	{
	}

	//////////////////////////////

	null_pointer::null_pointer()
		: std::logic_error("Attempting to dereference as null pointer.")
	{
	}

	null_pointer::null_pointer(const std::string& what_arg)
		: std::logic_error(what_arg)
	{
	}

	null_pointer::null_pointer(const char* what_arg)
		: std::logic_error(what_arg)
	{
	}

	//////////////////////////////

	allocation_error::allocation_error()
		: std::runtime_error("Resource allocation error.")
	{
	}

	allocation_error::allocation_error(const std::string& what_arg)
		: std::runtime_error(what_arg)
	{
	}

	allocation_error::allocation_error(const char* what_arg)
		: std::runtime_error(what_arg)
	{
	}

	////////////////////////////////////

	not_supported::not_supported()
		: std::runtime_error("Operation not supported.")
	{
	}

	not_supported::not_supported(const char* message)
		: std::runtime_error(message)
	{
	}

	not_supported::not_supported(const std::string& message)
		: std::runtime_error(message)
	{
	}

	////////////////////////////////////

	permission_denied::permission_denied()
		: std::runtime_error("Permission denied.")
	{
	}

	permission_denied::permission_denied(const char* message)
		: std::runtime_error(message)
	{
	}

	permission_denied::permission_denied(const std::string& message)
		: std::runtime_error(message)
	{
	}

	////////////////////////////////////

	unknown_error::unknown_error()
		: std::runtime_error("Unknown Error")
	{
	}

	unknown_error::unknown_error(const char* message)
		: std::runtime_error(message)
	{
	}

	unknown_error::unknown_error(const std::string& message)
		: std::runtime_error(message)
	{
	}
}