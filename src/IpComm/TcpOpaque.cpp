#include "TcpOpaque.h"

namespace StdExt::IpComm
{
#ifdef _WIN32
	WsaHandle::WsaHandle()
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	WsaHandle::~WsaHandle()
	{
		WSACleanup();
	}
#endif // _WIN32

	Endpoint getSocketEndpoint(SOCKET sock, IpVersion version)
	{
		Endpoint ret{};
		
		if (IpVersion::V4 == version)
		{
			sockaddr_in sockAddr4;
			memset(&sockAddr4, 0, sizeof(sockaddr_in));

			int addrLength = sizeof(sockaddr_in);

			getsockname(sock, (sockaddr*)&sockAddr4, &addrLength);

			ret.address = IpAddress(sockAddr4.sin_addr);
			ret.port = htons(sockAddr4.sin_port);
		}
		else if (IpVersion::V6 == version)
		{
			sockaddr_in6 sockAddr6;
			memset(&sockAddr6, 0, sizeof(sockaddr_in6));

			int addrLength = sizeof(sockaddr_in6);

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
		return errno
	#endif
	}
}