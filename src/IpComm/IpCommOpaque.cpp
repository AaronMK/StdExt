#include "IpCommOpaque.h"

#include <StdExt/IpComm/Exceptions.h>
#include <StdExt/IpComm/SockAddr.h>

#include <iostream>
#include <system_error>

#include <fcntl.h>

namespace StdExt::IpComm
{
#ifdef _WIN32
	PlatformCommsHandle::PlatformCommsHandle()
	{
		mSuccess = false;
		
		WSADATA wsaData;

		if ( 0 != WSAStartup(MAKEWORD(2, 2), &wsaData) )
			throw std::runtime_error("Windows sockets faild to initialize.");

		mSuccess = true;
	}

	PlatformCommsHandle::~PlatformCommsHandle()
	{
		if ( mSuccess )
			WSACleanup();
	}
#endif

	error_t getLastError()
	{
		#ifdef _WIN32
			return WSAGetLastError();
		#else
			return errno;
		#endif
	}

	void throwDefaultError(error_t  error_code)
	{
		switch( error_code )
		{
		case SOCK_ERR(ENETDOWN):
			throw InternalSubsystemFailure("Network is down.");
		case SOCK_ERR(ENOTSOCK):
			throw std::invalid_argument("Not a socket.");
		case SOCK_ERR(EINVAL):
			throw std::invalid_argument("Invalid argument on socket function.");
		case SOCK_ERR(ECONNABORTED):
			throw ConnectionAborted();
		case SOCK_ERR(ESHUTDOWN):
			throw ConnectionShutdown();
		case SOCK_ERR(ENOTCONN):
			throw NotConnected();
		case SOCK_ERR(ECONNRESET):
			throw ConnectionReset();
		case SOCK_ERR(EMSGSIZE):
			throw MessageTooBig();
		case SOCK_ERR(ENETUNREACH):
			throw NetworkUnreachable();
		case SOCK_ERR(EHOSTUNREACH):
			throw HostUnreachable();
		case SOCK_ERR(EADDRNOTAVAIL):
			throw InvalidIpAddress();
		case SOCK_ERR(EACCES):
		#ifndef _WIN32
		case SOCK_ERR(EPERM):
		#endif
			throw permission_denied("Permission denied on a socket operation.");
		case SOCK_ERR(EADDRINUSE):
			throw EndpointInUse();
		case SOCK_ERR(ECONNREFUSED):
			throw ConnectionRejected();
		case SOCK_ERR(EISCONN):
			throw AlreadyConnected();
		case SOCK_ERR(ETIMEDOUT):
			throw TimeOut();
		case SOCK_ERR(ENETRESET):
			throw TimeToLiveExpired();
		case SOCK_ERR(EOPNOTSUPP):
			throw not_supported("Operation on socket not supported");
		case SOCK_ERR(ENOBUFS):
		#ifndef STD_EXT_WIN32
		case SOCK_ERR(ENOMEM):
		#endif
			throw allocation_error("Failed to aquire memory for socket creation.");
		case SOCK_ERR(EMFILE):
			throw allocation_error(
				"The per-process limit on the number of open file"
				" descriptors has been reached when making a socket."
			);
		#ifndef STD_EXT_WIN32
		case SOCK_ERR(ENFILE):
			throw allocation_error(
				"The system wide limit on the total number of open files"
				" has been reached when making a socket."
			);
		#endif
		case SOCK_ERR(EBADF):
			throw std::invalid_argument("Argument is not a valid descriptor");
			
		#ifndef STD_EXT_WIN32
		case SOCK_ERR(EAGAIN):
		#else
		case SOCK_ERR(EWOULDBLOCK)
		#endif
			throw NonBlocking();
		default:
			throw unknown_error();
		}
	}

	Endpoint getSocketEndpoint(SOCKET sock)
	{
		SockAddr  sock_addr;
		auto result = getsockname(sock, sock_addr.data(), sock_addr.sizeInOut() );

		if (INVALID_SOCKET == result)
			throwDefaultError(getLastError());

		return sock_addr.toEndpoint();
	}

	SOCKET makeSocket(int domain, int type, int protocol)
	{
		#ifdef _WIN32
			SOCKET result = WSASocket(domain, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
		#else
			SOCKET result = socket(domain, type, protocol);
		#endif

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
			default:
				throwDefaultError(error_code);
			}
		}

		try
		{
			setSocketOption<int>(result, SOL_SOCKET, SO_REUSEADDR, 1);
			setSocketBlocking(result, true);
		}
		catch(const std::exception& e)
		{
			closeSocket(result);
			throw;
		}

		return result;
	}

	void setSocketBlocking(SOCKET socket, bool blocking)
	{
		#ifdef STD_EXT_WIN32
		u_long non_blocking = blocking ? 0 : 1;
		ioctlsocket(socket, FIONBIO, &non_blocking);
		#else
		int flags = fcntl(socket, F_GETFL, 0);
		flags = ( blocking ) ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
		auto result = fcntl(socket, F_SETFL, flags);

		if ( -1 == result )
			throwDefaultError( getLastError() );
		#endif
	}

	void connectSocket(
		SOCKET socket, const struct sockaddr* addr,
		socklen_t addrlen
	)
	{
		#ifdef _WIN32
			auto result = WSAConnect(socket, addr, addrlen, NULL, NULL, NULL, NULL);
		#else
			auto result = ::connect(socket, addr, addrlen);
		#endif

		if (0 != result)
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
			default:
				throwDefaultError(error_code);
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
			default:
				throwDefaultError(error_code);
			}
		}
	}

	void sendSocket(SOCKET socket, const void* data, size_t size)
	{
		auto result = ::send(socket, access_as<const char*>(data), Number::convert<int>(size), 0);

		if ( SOCKET_ERROR == result )
		{
			auto error_code = getLastError();
			switch (error_code)
			{
			case SOCK_ERR(ETIMEDOUT):
				throw NotConnected();
			default:
				throwDefaultError(error_code);
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
			case SOCK_ERR(EOPNOTSUPP):
				throw not_supported("Socket type does not support listening.");
			default:
				throwDefaultError(error_code);
			}
		}
	}

	SOCKET acceptSocket(SOCKET socket, sockaddr* remote_addr, socklen_t* addr_len)
	{
		SOCKET sock = accept(socket, remote_addr, addr_len);

		if (INVALID_SOCKET == sock)
			throwDefaultError(getLastError());

		setSocketOption<int>(sock, SOL_SOCKET, SO_REUSEADDR, 1);
		setSocketBlocking(sock, true);

		return sock;
	}

	size_t recvSocket(SOCKET socket, void* destination, size_t byteLength, int flags)
	{
		auto readResult = recv(
			socket,
			access_as<char*>(destination),
			Number::convert<int>(byteLength),
			flags
		);

		if (readResult < 0)
		{
			auto error_code = getLastError();
			switch (error_code)
			{
			case SOCK_ERR(EWOULDBLOCK):
			case SOCK_ERR(ECONNABORTED):
			case SOCK_ERR(ETIMEDOUT):
				throw TimeOut();
			case SOCK_ERR(EOPNOTSUPP):
				throw not_supported("Unsupported options passed to recv().");
			default:
				throwDefaultError(error_code);
			}
		}

		return Number::convert<size_t>(readResult);
	}

	void sendTo(
		SOCKET socket, const void* data, size_t size,
		const struct sockaddr* dest_addr, socklen_t addrlen
	)
	{
		auto result = sendto(
			socket, access_as<const char*>(data),
			Number::convert<int>(size), 0,
			dest_addr, addrlen
		);
		
		if ( result < 0 )
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
			default:
				throwDefaultError(error_code);
			}
		}
	}

	size_t receiveFrom(
		SOCKET socket, void* data, size_t size,
		sockaddr* from_addr, socklen_t* from_len
	)
	{
		
		#if defined(STD_EXT_APPLE) || defined(STD_EXT_GCC)
		auto result = recvfrom(socket, data, size, 0, from_addr, from_len);
		#else
		int int_from_len = Number::convert<int>(*from_len);

		auto result = recvfrom(
			socket, access_as<char*>(data), Number::convert<int>(size),
			0, from_addr,
			(from_addr) ? &int_from_len : nullptr
		);
		#endif

		if ( 0 == result || SOCKET_ERROR == result )
		{
			auto error_code = getLastError();

			switch (error_code)
			{
			case SOCK_ERR(EFAULT):
				throw std::runtime_error("Bad data address");
			default:
				throwDefaultError(error_code);
			}
		}

		#if !defined(STD_EXT_APPLE) && !defined(STD_EXT_GCC)
		if ( from_len )
			*from_len = Number::convert<socklen_t>(int_from_len);
		#endif

		return Number::convert<size_t>(result);
	}

	void shutdownSocket(SOCKET socket)
	{
		#if STD_EXT_WIN32
		auto result = shutdown(socket, SD_BOTH);
		#else
		auto result = shutdown(socket, SHUT_RDWR);
		#endif

		if ( 0 != result )
			throwDefaultError( getLastError() );
	}

	void closeSocket(SOCKET socket)
	{
		#ifdef STD_EXT_WIN32
		auto result = closesocket(socket);
		#else
		auto result = close(socket);
		#endif

		if ( 0 != result )
			throwDefaultError( getLastError() );
	}
}
