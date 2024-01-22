#ifndef _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_
#define _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_

#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/IpComm/Endpoint.h>

#include <StdExt/CallableTraits.h>

#include <array>

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN

#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	pragma comment (lib, "Ws2_32.lib")
#else
#	include <sys/ioctl.h>
#	include <sys/socket.h>

#	include <unistd.h>
#	include <errno.h>
#endif

namespace StdExt::IpComm
{
#ifdef STD_EXT_WIN32
	class PlatformCommsHandle
	{
	public:
		PlatformCommsHandle();
		~PlatformCommsHandle();

	private:
		bool mSuccess;
	};

#	define SOCK_ERR(name) WSA##name

	using error_t = decltype( WSAGetLastError() );
#else
	struct PlatformCommsHandle {};

	using SOCKET = int;
	static constexpr int INVALID_SOCKET = -1;

#	define SOCK_ERR(name) name

	using error_t = errno_t;
#endif 


#ifdef STD_EXT_APPLE
	static constexpr ssize_t SOCKET_ERROR = -1;
#endif

	/**
	 * @internal
	 *
	 * @brief
	 *  Opaque data and functionality for TcpConnection.
	 */
	struct TcpConnOpaque
	{
		TcpConnOpaque(const TcpConnOpaque&) = delete;
		TcpConnOpaque& operator=(const TcpConnOpaque&) = delete;

		TcpConnOpaque();
		TcpConnOpaque(IpAddress IP, Port port);

		~TcpConnOpaque();

		void setReceiveTimeout(const std::chrono::microseconds& timeout_period);

		PlatformCommsHandle Handle;
		SOCKET Socket;

		Endpoint Remote{};
		Endpoint Local{};
	};

	struct TcpServerOpaque
	{
		TcpServerOpaque(const TcpServerOpaque&) = delete;
		TcpServerOpaque& operator=(const TcpServerOpaque&) = delete;

		TcpServerOpaque(IpAddress addr, Port port);
		~TcpServerOpaque();

		PlatformCommsHandle Handle;
		SOCKET Socket = INVALID_SOCKET;

		Endpoint LocalEndpoint{};
	};
	
	Endpoint getSocketEndpoint(SOCKET sock);

	error_t getLastError();
	
	void throwDefaultError(error_t error_code);

	SOCKET makeSocket(int domain, int type, int protocol);

	void setSocketBlocking(SOCKET socket, bool blocking);

	void connectSocket(
		SOCKET socket, const struct sockaddr *addr,
		socklen_t addrlen
	);

	void bindSocket(
		SOCKET socket, const struct sockaddr *addr,
		socklen_t addrlen
	);

	void sendSocket(SOCKET socket, const void* data, size_t size);

	void listenSocket(SOCKET socket, int backlog);

	SOCKET acceptSocket(SOCKET socket, sockaddr* remote_addr, socklen_t* addr_len);

	size_t recvSocket(
		SOCKET socket, void* destination,
		size_t byteLength, int flags
	);

	void sendTo(
		SOCKET socket, const void* data, size_t size,
		const struct sockaddr *dest_addr, socklen_t addrlen
	);

	size_t receiveFrom(
		SOCKET socket, void* data, size_t size,
		sockaddr *from_addr, socklen_t* from_len
	);

	void shutdownSocket(SOCKET socket);

	void closeSocket(SOCKET socket);

	template<typename T>
	void setSocketOption(SOCKET socket, int level, int option_name, T option_value)
	{
		auto result = setsockopt(
			socket, level, option_name,
			access_as<const void*>(&option_value), sizeof(T)
		);

		if ( 0 != result )
			throwDefaultError( getLastError() );
	}
}

#endif// _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_