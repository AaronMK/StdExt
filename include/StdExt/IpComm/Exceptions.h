#ifndef _STD_EXT__IP_COMM_EXCEPTIONS_H_
#define _STD_EXT__IP_COMM_EXCEPTIONS_H_

#include "../StdExt.h"
#include "IpComm.h"

#include <cstdint>
#include <stdexcept>

namespace StdExt::IpComm
{
	class STD_EXT_EXPORT IpCommException : public std::runtime_error
	{
	protected:
		IpCommException();
		IpCommException(const char* message);
		IpCommException(const std::string& message);
	};
	
	/**
	 * @brief
	 *  The operation did not complete within a prescribed amount of time.
	 */
	class STD_EXT_EXPORT TimeOut : public IpCommException
	{
	public:
		TimeOut();
		TimeOut(const char* message);
		TimeOut(const std::string& message);
	};

	/**
	 * @brief
	 *  The operation failed because a connection has not been established.
	 */
	class STD_EXT_EXPORT NotConnected : public IpCommException
	{
	public:
		NotConnected();
		NotConnected(const char* message);
		NotConnected(const std::string& message);
	};

	/**
	 * @brief
	 *  The operation failed because the server is not listening.
	 */
	class STD_EXT_EXPORT NotListening : public IpCommException
	{
	public:
		NotListening();
		NotListening(const char* message);
		NotListening(const std::string& message);
	};

	/**
	 * @brief
	 *  The operation was failed because it was invalid for
	 *  an object already in the connected state.
	 */
	class STD_EXT_EXPORT AlreadyConnected : public IpCommException
	{
	public:
		AlreadyConnected();
		AlreadyConnected(const char* message);
		AlreadyConnected(const std::string& message);
	};

	/**
	 * @brief
	 *  The IP Address passed was not valid.  For servers this usually means a
	 *  passed address does not have an associated IP Interface.
	 */
	class STD_EXT_EXPORT InvalidIpAddress : public IpCommException
	{
	public:
		InvalidIpAddress();
		InvalidIpAddress(const char* message);
		InvalidIpAddress(const std::string& message);
	};

	/**
	 * @brief
	 *  There was a failure of the internal socketing subsystem.
	 */
	class STD_EXT_EXPORT InternalSubsystemFailure : public IpCommException
	{
	public:
		InternalSubsystemFailure();
		InternalSubsystemFailure(const char* message);
		InternalSubsystemFailure(const std::string& message);
	};

	/**
	 * @brief
	 *  The buffer passed to receive data was not valid.  Usually this
	 *  means the pointer was null.
	 */
	class STD_EXT_EXPORT InvalidBuffer : public IpCommException
	{
	public:
		InvalidBuffer();
		InvalidBuffer(const char* message);
		InvalidBuffer(const std::string& message);
	};

	/**
	 * @brief
	 *  The connection was rejected by the remote server.
	 */
	class STD_EXT_EXPORT ConnectionRejected : public IpCommException
	{
	public:
		ConnectionRejected();
		ConnectionRejected(const char* message);
		ConnectionRejected(const std::string& message);
	};

	/**
	 * @brief
	 *  The connection was reset by the remote server.
	 */
	class STD_EXT_EXPORT ConnectionReset : public IpCommException
	{
	public:
		ConnectionReset();
		ConnectionReset(const char* message);
		ConnectionReset(const std::string& message);
	};

	/**
	 * @brief
	 *  The IP/Port combination passed to create a server is already
	 *  in use.
	 */
	class STD_EXT_EXPORT EndpointInUse : public IpCommException
	{
	public:
		EndpointInUse();
		EndpointInUse(const char* message);
		EndpointInUse(const std::string& message);
	};

	/**
	 * @brief
	 *  Network cannot be reached.
	 */
	class STD_EXT_EXPORT NetworkUnreachable : public IpCommException
	{
	public:
		NetworkUnreachable();
		NetworkUnreachable(const char* message);
		NetworkUnreachable(const std::string& message);
	};
}

#endif // _STD_EXT__IP_COMM_EXCEPTIONS_H_