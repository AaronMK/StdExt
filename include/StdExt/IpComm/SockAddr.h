#ifndef _STD_EXT_IP_COMM_SOCK_ADDR_H_
#define _STD_EXT_IP_COMM_SOCK_ADDR_H_

#include "IpAddress.h"

#include <array>

namespace StdExt::IpComm
{
	class STD_EXT_EXPORT SockAddr
	{
	private:
		std::array<std::byte,
			std::max( sizeof(sockaddr_in), sizeof(sockaddr_in6) )
		> mData;

		socklen_t mSize;

	public:
		SockAddr();

		SockAddr(const in_addr& addr, Port port);
		SockAddr(const in6_addr& addr, Port port);

		SockAddr(const Endpoint& endpoint);
		SockAddr(const IpAddress& addr, Port port);

		const sockaddr* data() const;
		socklen_t size() const;

		Endpoint toEndpoint() const;
	};
}

#endif // !_STD_EXT_IP_COMM_SOCK_ADDR_H_