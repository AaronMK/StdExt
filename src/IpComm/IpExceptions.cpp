#include <StdExt/IpComm/Exceptions.h>

using namespace std;

namespace StdExt::IpComm
{
	IpCommException::IpCommException()
		: runtime_error("IP Exception")
	{
	}

	IpCommException::IpCommException(const char* message)
		: runtime_error(message)
	{
	}

	IpCommException::IpCommException(const std::string& message)
		: runtime_error(message)
	{
	}

	////////////////////////////////////

	TimeOut::TimeOut()
		: IpCommException("The operation did not complete within a prescribed amount of time.")
	{
	}

	TimeOut::TimeOut(const char* message)
		: IpCommException(message)
	{
	}

	TimeOut::TimeOut(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	NotConnected::NotConnected()
		: IpCommException("The operation failed because a connection has not been established.")
	{
	}

	NotConnected::NotConnected(const char* message)
		: IpCommException(message)
	{
	}

	NotConnected::NotConnected(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	NotListening::NotListening()
		: IpCommException("The operation failed because the server is not listening.")
	{
	}

	NotListening::NotListening(const char* message)
		: IpCommException(message)
	{
	}

	NotListening::NotListening(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	AlreadyConnected::AlreadyConnected()
		: IpCommException("Already connected.")
	{
	}

	AlreadyConnected::AlreadyConnected(const char* message)
		: IpCommException(message)
	{
	}

	AlreadyConnected::AlreadyConnected(const std::string& message)
		: IpCommException(message)
	{

	}

	////////////////////////////////////

	InvalidIpAddress::InvalidIpAddress()
		: IpCommException("Invalid IP Address.")
	{
	}

	InvalidIpAddress::InvalidIpAddress(const char* message)
		: IpCommException(message)
	{

	}

	InvalidIpAddress::InvalidIpAddress(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	InternalSubsystemFailure::InternalSubsystemFailure()
		: IpCommException("Failure of the internal socketing subsystem.")
	{
	}

	InternalSubsystemFailure::InternalSubsystemFailure(const char* message)
		: IpCommException(message)
	{
	}

	InternalSubsystemFailure::InternalSubsystemFailure(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	InvalidBuffer::InvalidBuffer()
		: IpCommException("The buffer passed to receive data was not valid.")
	{
	}

	InvalidBuffer::InvalidBuffer(const char* message)
		: IpCommException(message)
	{
	}

	InvalidBuffer::InvalidBuffer(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	ConnectionRejected::ConnectionRejected()
		: IpCommException("The connection was rejected by the remote server.")
	{
	}

	ConnectionRejected::ConnectionRejected(const char* message)
		: IpCommException(message)
	{
	}

	ConnectionRejected::ConnectionRejected(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	ConnectionReset::ConnectionReset()
		: IpCommException("The connection was reset by the remote server.")
	{
	}

	ConnectionReset::ConnectionReset(const char* message)
		: IpCommException(message)
	{
	}

	ConnectionReset::ConnectionReset(const std::string& message)
		: IpCommException(message)
	{
	}

	////////////////////////////////////

	EndpointInUse::EndpointInUse()
		: IpCommException("The IP/Port combination passed to create a server is already in use.")
	{
	}

	EndpointInUse::EndpointInUse(const char* message)
		: IpCommException(message)
	{
	}

	EndpointInUse::EndpointInUse(const std::string& message)
		: IpCommException(message)
	{
	}

	//////////////////////////////////////

	HostUnreachable::HostUnreachable()
		: IpCommException("Host is unreachable.")
	{
	}

	HostUnreachable::HostUnreachable(const char* message)
		: IpCommException(message)
	{
	}

	HostUnreachable::HostUnreachable(const std::string& message)
		: IpCommException(message)
	{
	}

	//////////////////////////////////////

	NetworkUnreachable::NetworkUnreachable()
		: IpCommException("Network is unreachable.")
	{
	}

	NetworkUnreachable::NetworkUnreachable(const char* message)
		: IpCommException(message)
	{
	}

	NetworkUnreachable::NetworkUnreachable(const std::string& message)
		: IpCommException(message)
	{
	}

	//////////////////////////////////////

	MessageTooBig::MessageTooBig()
		: IpCommException("Message was too big to send.")
	{
	}

	MessageTooBig::MessageTooBig(const char* message)
		: IpCommException(message)
	{
	}

	MessageTooBig::MessageTooBig(const std::string& message)
		: IpCommException(message)
	{
	}

	//////////////////////////////////////

	TimeToLiveExpired::TimeToLiveExpired()
		: IpCommException("Time to live has expired.")
	{
	}

	TimeToLiveExpired::TimeToLiveExpired(const char* message)
		: IpCommException(message)
	{
	}

	TimeToLiveExpired::TimeToLiveExpired(const std::string& message)
		: IpCommException(message)
	{
	}
}