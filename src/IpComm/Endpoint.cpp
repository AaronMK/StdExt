#include <StdExt/IpComm/Endpoint.h>

#include <StdExt/Compare.h>

namespace StdExt::IpComm
{
	Endpoint::Endpoint()
	{
		port = 0;
	}

	Endpoint::Endpoint(const sockaddr* addr, socklen_t len)
		: Endpoint()
	{
		static_assert( sizeof(sockaddr_in) != sizeof(sockaddr_in6) );

		if ( len == sizeof(sockaddr_in) )
		{
			sockaddr_in* sockAddr4 = access_as<sockaddr_in*>(addr);

			address = IpAddress(sockAddr4->sin_addr);
			port = ntohs(sockAddr4->sin_port);
		}
		else if ( len == sizeof(sockaddr_in6) )
		{
			sockaddr_in6* sockAddr6 = access_as<sockaddr_in6*>(addr);

			address = IpAddress(sockAddr6->sin6_addr);
			port = ntohs(sockAddr6->sin6_port);
		}
	}
		
	Endpoint::Endpoint(const IpAddress& _addr, Port _port)
		: address(_addr), port(_port)
	{
	}

	Endpoint::Endpoint(IpVersion _version, Port _port)
		: address(IpAddress::any(_version)), port(_port)
	{
	}

	std::strong_ordering Endpoint::operator<=>(const Endpoint& other) const
	{
		int comp_result = compare(
			port, other.port,
			address, other.address
		);

		if ( comp_result < 0 )
			return std::strong_ordering::less;
		else if ( comp_result > 0 )
			return std::strong_ordering::greater;

		return std::strong_ordering::equal;
	}

	bool Endpoint::operator==(const Endpoint& other) const
	{
		return (0 == compare(
			port, other.port,
			address, other.address
		));
	}
}