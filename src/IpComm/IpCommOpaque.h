#ifndef _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_
#define _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_

#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/IpComm/Endpoint.h>

#include <array>

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN

#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	pragma comment (lib, "Ws2_32.lib")
#else
#	include <sys/ioctl.h>
#endif // _WIN32

namespace StdExt::IpComm
{
#ifdef _WIN32
	class WsaHandle
	{
	public:
		WsaHandle();
		~WsaHandle();

	private:
		bool mSuccess;
	};
#else
	struct WsaHandle {};

	using SOCKET = int;
	static constexpr int INVALID_SOCKET = -1;
#endif 

	
	/**
	 * @internal
	 *
	 * @brief
	 *  Opaque data and functionality for TcpConnection.
	 */
	struct TcpConnOpaque
	{
		WsaHandle Handle;
		SOCKET Socket = INVALID_SOCKET;

		Endpoint Remote{};
		Endpoint Local{};
	};

	struct TcpServerOpaque
	{
		WsaHandle Handle;
		SOCKET Socket = INVALID_SOCKET;

		Endpoint LocalEndpoint{};
	};
	
	Endpoint getSocketEndpoint(SOCKET sock);

	int getLastError();

	SOCKET makeSocket(int domain, int type, int protocol);

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
}

#endif// _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_