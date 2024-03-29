#include <StdExt/IpComm/SockAddr.h>
#include <StdExt/IpComm/Endpoint.h>

namespace StdExt::IpComm
{
	SockAddr::SockAddr()
	{
		memset(&mData[0], 0, mData.size());
		mSize = 0;
	}
	
	SockAddr::SockAddr(const sockaddr* addr)
		: SockAddr()
	{
		if ( AF_INET == addr->sa_family )
		{
			mSize = Number::convert<socklen_t>(sizeof(sockaddr_in));
			memcpy(mData.data(), addr, mSize);
		}
		else if ( AF_INET6 == addr->sa_family )
		{
			mSize = Number::convert<socklen_t>(sizeof(sockaddr_in6));
			memcpy(mData.data(), addr, mSize);
		}
	}

	
	SockAddr::SockAddr(const sockaddr_in* addr)
		: SockAddr()
	{
		mSize = Number::convert<socklen_t>(sizeof(sockaddr_in));
		memcpy(mData.data(), addr, mSize);
	}

	SockAddr::SockAddr(const sockaddr_in6* addr)
		: SockAddr()
	{
		mSize = Number::convert<socklen_t>(sizeof(sockaddr_in6));
		memcpy(mData.data(), addr, mSize);
	}

	SockAddr::SockAddr(const in_addr& addr, Port port)
		: SockAddr()
	{
		sockaddr_in* sockAddr4 = access_as<sockaddr_in*>(&mData[0]);
		
		sockAddr4->sin_family = AF_INET;
		sockAddr4->sin_port = htons(port);
		sockAddr4->sin_addr = addr;
	}

	SockAddr::SockAddr(const in6_addr& addr, Port port)
		: SockAddr()
	{
		sockaddr_in6* sockAddr6 = access_as<sockaddr_in6*>(&mData[0]);
		
		sockAddr6->sin6_family = AF_INET6;
		sockAddr6->sin6_port =  htons(port);
		sockAddr6->sin6_addr = addr;
	}

	SockAddr::SockAddr(const Endpoint& endpoint)
		: SockAddr()
	{
		assert( endpoint.address.isValid() );

		if (endpoint.address.version() == IpVersion::V4)
		{
			sockaddr_in* sockAddr4 = access_as<sockaddr_in*>(&mData[0]);
				
			sockAddr4->sin_family = AF_INET;
			sockAddr4->sin_port = htons(endpoint.port);
			sockAddr4->sin_addr = endpoint.address.getSysIPv4();

			mSize = Number::convert<socklen_t>(sizeof(sockaddr_in));
		}
		else if (endpoint.address.version() == IpVersion::V6)
		{
			sockaddr_in6* sockAddr6 = access_as<sockaddr_in6*>(&mData[0]);

			sockAddr6->sin6_family = AF_INET6;
			sockAddr6->sin6_port = htons(endpoint.port);
			sockAddr6->sin6_addr = endpoint.address.getSysIPv6();

			mSize = Number::convert<socklen_t>(sizeof(sockaddr_in6));
		}
	}

	SockAddr::SockAddr(const IpAddress& addr, Port port)
		: SockAddr( Endpoint(addr, port))
	{
	}

	const sockaddr* SockAddr::data() const
	{
		return access_as<const sockaddr*>(&mData[0]);
	}
	
	sockaddr* SockAddr::data()
	{
		return access_as<sockaddr*>(&mData[0]);
	}

	socklen_t SockAddr::size() const
	{
		return mSize;
	}

	socklen_t* SockAddr::sizeInOut()
	{
		mSize = Number::convert<socklen_t>(mData.size());
		return &mSize;
	}
	
	IpVersion SockAddr::version() const
	{
		if ( mSize == sizeof(sockaddr_in) )
			return IpVersion::V4;
		else if ( mSize == sizeof(sockaddr_in6) )
			return IpVersion::V6;
		else
			return IpVersion::NONE;
	}

	Endpoint SockAddr::toEndpoint() const
	{
		static_assert( sizeof(sockaddr_in) != sizeof(sockaddr_in6) );

		if ( mSize == sizeof(sockaddr_in) )
		{
			sockaddr_in* sockAddr4 = access_as<sockaddr_in*>(&mData[0]);

			return Endpoint(
				IpAddress(sockAddr4->sin_addr),
				ntohs(sockAddr4->sin_port)
			);
		}
		else if ( mSize == sizeof(sockaddr_in6) )
		{
			sockaddr_in6* sockAddr6 = access_as<sockaddr_in6*>(&mData[0]);

			return Endpoint(
				IpAddress(sockAddr6->sin6_addr),
				ntohs(sockAddr6->sin6_port)
			);
		}

		return Endpoint();
	}
}