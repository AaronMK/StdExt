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
}