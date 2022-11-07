#ifndef _STD_EXT_IP_COMM_ENDPOINT_H_
#define _STD_EXT_IP_COMM_ENDPOINT_H_

#include "IpAddress.h"

namespace StdExt::IpComm
{
	struct STD_EXT_EXPORT Endpoint
	{
		IpAddress address;
		Port      port;

		Endpoint();
		Endpoint(const sockaddr* addr, socklen_t len);
		Endpoint(const IpAddress& _addr, Port _port);

		/**
		 * @brief
		 *  Creates an end point that denotes a specific port
		 *  but on any interface.
		 */
		Endpoint(IpVersion _version, Port _port);
	};
}

#endif // !_STD_EXT_IP_COMM_ENDPOINT_H_