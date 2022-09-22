#include "IpCommOpaque.h"

#include <StdExt/IpComm/Exceptions.h>

#include <system_error>

namespace StdExt::IpComm
{
#ifdef _WIN32
	WsaHandle::WsaHandle()
	{
		mSuccess = false;
		
		WSADATA wsaData;

		if ( 0 != WSAStartup(MAKEWORD(2, 2), &wsaData) )
			throw std::runtime_error("Windows sockets faild to initialize.");

		mSuccess = true;
	}

	WsaHandle::~WsaHandle()
	{
		if ( mSuccess )
			WSACleanup();
	}

#	define SOCK_ERR(name) WSA##name
#else
#	define SOCK_ERR(name) name
#endif

	Endpoint getSocketEndpoint(SOCKET sock, IpVersion version)
	{
		Endpoint ret{};
		
		if (IpVersion::V4 == version)
		{
			sockaddr_in sockAddr4;
			memset(&sockAddr4, 0, sizeof(sockaddr_in));

			socklen_t addrLength = sizeof(sockaddr_in);

			getsockname(sock, (sockaddr*)&sockAddr4, &addrLength);

			ret.address = IpAddress(sockAddr4.sin_addr);
			ret.port = htons(sockAddr4.sin_port);
		}
		else if (IpVersion::V6 == version)
		{
			sockaddr_in6 sockAddr6;
			memset(&sockAddr6, 0, sizeof(sockaddr_in6));

			socklen_t addrLength = sizeof(sockaddr_in6);

			getsockname(sock, (sockaddr*)&sockAddr6, &addrLength);

			ret.address = IpAddress(sockAddr6.sin6_addr);
			ret.port = htons(sockAddr6.sin6_port);
		}

		return ret;
	}

	int getLastError()
	{
	#ifdef _WIN32
		return WSAGetLastError();
	#else
		return errno;
	#endif
	}

	SOCKET makeSocket(int domain, int type, int protocol)
	{
		SOCKET result = socket(domain, type, protocol);

		if (INVALID_SOCKET == result)
		{
			auto error_code = getLastError();
			switch( error_code )
			{
			case SOCK_ERR(EACCES):
				throw permission_denied("Permission denied when making a socket.");
			case SOCK_ERR(EAFNOSUPPORT):
			case SOCK_ERR(EPROTONOSUPPORT):
				throw not_supported("Operation not supported when making a socket.");
			case SOCK_ERR(EINVAL):
				throw std::invalid_argument("Unknown protocol or invalid flags.");
			case SOCK_ERR(ENOBUFS):
			#ifndef _WIN32
			case SOCK_ERR(ENOMEM):
			#endif
				throw allocation_error("Failed to aquire memory for socket creation.");
			case SOCK_ERR(EMFILE):
				throw allocation_error(
					"The per-process limit on the number of open file"
					" descriptors has been reached when making a socket."
				);
			#ifndef _WIN32
			case SOCK_ERR(ENFILE):
				throw allocation_error(
					"The system wide limit on the total number of open files"
					" has been reached when making a socket."
				);
			#endif
			default:
				throw std::system_error(error_code, std::generic_category());
			}
		}

		return result;
	}
	void connectSocket(
		SOCKET socket, const struct sockaddr* addr,
		socklen_t addrlen
	)
	{
		auto result = ::connect(socket, addr, addrlen);

		if (0 > result)
		{
			auto error_code = getLastError();
			switch (error_code)
			{
			case SOCK_ERR(EACCES):
			#ifndef _WIN32
			case SOCK_ERR(EPERM):
			#endif
				throw permission_denied(
					"Permission denied and/or braodcast flag was not set for"
					" a broadcast connection."
				);
			case SOCK_ERR(EADDRINUSE):
				throw EndpointInUse();
			case SOCK_ERR(ECONNREFUSED):
				throw ConnectionRejected();
			case SOCK_ERR(EISCONN):
				throw AlreadyConnected();
			case SOCK_ERR(ETIMEDOUT):
				throw TimeOut();
			case SOCK_ERR(ENETUNREACH):
				throw NetworkUnreachable();
			default:
				throw std::system_error(error_code, std::generic_category());
			}
		}
	}

	void bindSocket(SOCKET socket, const sockaddr* addr, socklen_t addrlen)
	{
		auto result = ::bind(socket, addr, addrlen);
		
		if (0 > result)
		{
			auto error_code = getLastError();
			switch (error_code)
			{
			case SOCK_ERR(EACCES):
				throw permission_denied(
					"The address is protected and the user does not have required permissions."
				);
			case SOCK_ERR(EADDRINUSE):
				throw EndpointInUse();
			case SOCK_ERR(EINVAL):
				throw AlreadyConnected();
			default:
				throw std::system_error(error_code, std::generic_category());
			}
		}
	}

	void listenSocket(SOCKET socket, int backlog)
	{
		auto result = ::listen(socket, backlog);
		
		if (0 > result)
		{
			auto error_code = getLastError();
			switch (error_code)
			{
			case SOCK_ERR(EADDRINUSE):
				throw EndpointInUse();
			case SOCK_ERR(EOPNOTSUPP):
				throw not_supported("Socket type does not support listening.");
			default:
				throw std::system_error(error_code, std::generic_category());
			}
		}
	}

	size_t recvSocket(SOCKET socket, void* destination, size_t byteLength, int flags)
	{
		
		int readResult = recv(
			socket,
			access_as<char*>(destination),
			Number::convert<int>(byteLength),
			flags
		);

		if (readResult < 0)
		{
			switch (getLastError())
			{
			case SOCK_ERR(ENOTCONN):
				throw NotConnected();
			case SOCK_ERR(ECONNABORTED):
			case SOCK_ERR(ETIMEDOUT):
				throw TimeOut();
			case SOCK_ERR(EOPNOTSUPP):
				throw not_supported("Unsupported options passed to recv().");
			default:
				throw unknown_error();
			}
		}

		return Number::convert<size_t>(readResult);
	}
}