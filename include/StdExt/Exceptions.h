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

	/**
	 * @brief
	 *  Operation is not valid given the current system or object state.
	 */
	class invalid_operation : public std::logic_error
	{
	public:
		invalid_operation(const std::string& what_arg);
		invalid_operation(const char* what_arg);
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
	
	/**
	 * @brief
	 *  A failure to allocate a resource.  It does not have to necessarily
	 *  be memory, and allows a descriptive message to be passed.
	 */
	class allocation_error : public std::runtime_error
	{
	public:
		allocation_error();
		allocation_error(const std::string& what_arg);
		allocation_error(const char* what_arg);
	};

	/**
	 * @brief
	 *  Operation is not supported on the host system or called object
	 *  independent of object or system state.
	 */
	class not_supported : public std::runtime_error
	{
	public:
		not_supported();
		not_supported(const char* message);
		not_supported(const std::string& message);
	};

	/**
	 * @brief
	 *  Application or user does not have sufficient privileges to 
	 *  compete an operation.
	 */
	class permission_denied : public std::runtime_error
	{
	public:
		permission_denied();
		permission_denied(const char* message);
		permission_denied(const std::string& message);
	};

	/**
	 * @brief
	 *  Unknown Error
	 */
	class unknown_error : public std::runtime_error
	{
	public:
		unknown_error();
		unknown_error(const char* message);
		unknown_error(const std::string& message);
	};
}

#endif // _STD_EXT_EXCEPTIONS_H_