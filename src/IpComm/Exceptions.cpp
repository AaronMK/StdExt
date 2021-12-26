#include <StdExt/IpComm/Exceptions.h>

using namespace std;

namespace StdExt::IpComm
{
	TimeOut::TimeOut()
		: runtime_error("The operation did not complete within a prescribed amount of time.")
	{

	}

	TimeOut::TimeOut(const char* message)
		: runtime_error(message)
	{

	}

	TimeOut::TimeOut(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	NotConnected::NotConnected()
		: runtime_error("The operation failed because a connection has not been established.")
	{

	}

	NotConnected::NotConnected(const char* message)
		: runtime_error(message)
	{

	}

	NotConnected::NotConnected(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	NotListening::NotListening()
		: runtime_error("The operation failed because the server is not listening.")
	{

	}

	NotListening::NotListening(const char* message)
		: runtime_error(message)
	{

	}

	NotListening::NotListening(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	AlreadyConnected::AlreadyConnected()
		: runtime_error("Already connected.")
	{

	}

	AlreadyConnected::AlreadyConnected(const char* message)
		: runtime_error(message)
	{

	}

	AlreadyConnected::AlreadyConnected(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	InvalidIpAddress::InvalidIpAddress()
		: runtime_error("Invalid IP Address.")
	{

	}

	InvalidIpAddress::InvalidIpAddress(const char* message)
		: runtime_error(message)
	{

	}

	InvalidIpAddress::InvalidIpAddress(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	InternalSubsystemFailure::InternalSubsystemFailure()
		: runtime_error("Failure of the internal socketing subsystem.")
	{

	}

	InternalSubsystemFailure::InternalSubsystemFailure(const char* message)
		: runtime_error(message)
	{

	}

	InternalSubsystemFailure::InternalSubsystemFailure(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	InvalidBuffer::InvalidBuffer()
		: runtime_error("The buffer passed to receive data was not valid.")
	{

	}

	InvalidBuffer::InvalidBuffer(const char* message)
		: runtime_error(message)
	{

	}

	InvalidBuffer::InvalidBuffer(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	ConnectionRejected::ConnectionRejected()
		: runtime_error("The connection was rejected by the remote server.")
	{

	}

	ConnectionRejected::ConnectionRejected(const char* message)
		: runtime_error(message)
	{

	}

	ConnectionRejected::ConnectionRejected(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	ConnectionReset::ConnectionReset()
		: runtime_error("The connection was reset by the remote server.")
	{

	}

	ConnectionReset::ConnectionReset(const char* message)
		: runtime_error(message)
	{

	}

	ConnectionReset::ConnectionReset(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	PortInUse::PortInUse()
		: runtime_error("The IP/Port combination passed to create a server is already in use.")
	{

	}

	PortInUse::PortInUse(const char* message)
		: runtime_error(message)
	{

	}

	PortInUse::PortInUse(const std::string& message)
		: runtime_error(message)
	{

	}

	////////////////////////////////////

	UnknownError::UnknownError()
		: runtime_error("Unknown Error")
	{

	}

	UnknownError::UnknownError(const char* message)
		: runtime_error(message)
	{

	}

	UnknownError::UnknownError(const std::string& message)
		: runtime_error(message)
	{

	}

}