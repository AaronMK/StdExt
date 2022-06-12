#ifndef _IP_COMM_TCP_CONN_OPAQUE_H_
#define _IP_COMM_TCP_CONN_OPAQUE_H_

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN

#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>

#	pragma comment (lib, "Ws2_32.lib")
#endif // _WIN32

#include <StdExt/IpComm/IpAddress.h>

namespace StdExt::IpComm
{
#ifdef _WIN32
	class WsaHandle
	{
	public:
		WsaHandle();
		~WsaHandle();
	};

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

		IpAddress RemoteIP;
		Port      RemotePort = 0;

		IpAddress LocalIP;
		Port      LocalPort = 0;
	};
#endif // _WIN32
}

#endif// _IP_COMM_TCP_CONN_OPAQUE_H_