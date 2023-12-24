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

#ifdef STD_EXT_APPLE
namespace std
{
	class format_error : public std::runtime_error
	{
	public:
		format_error(const std::string& what_arg);
		format_error(const char* what_arg);
		format_error(const format_error& other) noexcept;
	};
}
#endif

#endif // _STD_EXT_EXCEPTIONS_H_