#ifndef _STD_EXT_IP_COMM_H_
#define _STD_EXT_IP_COMM_H_

#include "../StdExt.h"

#include <cstdint>
#include <bit>

#ifdef _WIN32
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif

#	include <windows.h>
#	include <ws2tcpip.h>
#	include <In6addr.h>

#	pragma comment(lib, "ws2_32.lib")
#else
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif

/**
 * @todo
 *  Add UDP functionality.
 */
namespace StdExt::IpComm
{
	enum class IpVersion
	{
		NONE,
		V4,
		V6
	};

	typedef uint16_t Port;

	class SockAddr;
	class IpAddress;
	struct Endpoint;
}

#endif // _IP_COMM_H_