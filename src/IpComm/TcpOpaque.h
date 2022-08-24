#ifndef _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_
#define _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_

#include <StdExt/IpComm/IpAddress.h>

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN

#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	pragma comment (lib, "Ws2_32.lib")

static constexpr auto ENOTCONN = WSAENOTCONN;
static constexpr auto ECONNABORTED = WSAECONNABORTED;
static constexpr auto ETIMEDOUT = WSAETIMEDOUT;
static constexpr auto EINVAL = WSAEINVAL;
static constexpr auto ECONNRESET = WSAECONNRESET;
static constexpr auto SO_ERROR = SOCKET_ERROR;
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
	
	Endpoint getSocketEndpoint(SOCKET sock, IpVersion version);

	int getLastError();
}

#endif// _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_