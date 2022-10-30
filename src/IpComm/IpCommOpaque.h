#ifndef _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_
#define _STD_EXT_IP_COMM_TCP_CONN_OPAQUE_H_

#include <StdExt/IpComm/IpAddress.h>

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

	class SockAddr
	{
	private:
		std::array<char,
			std::max( sizeof(sockaddr_in), sizeof(sockaddr_in6) )
		> mData;

		socklen_t mSize = 0;

	public:
		SockAddr(const Endpoint& endpoint)
		{
			assert( endpoint.address.isValid() );
			
			memset(&mData[0], 0, mData.size());

			if (endpoint.address.version() == IpVersion::V4)
			{
				sockaddr_in* sockAddr4 = access_as<sockaddr_in*>(&mData[0]);
				
				sockAddr4->sin_family = AF_INET;
				sockAddr4->sin_port = htons(endpoint.port);
				sockAddr4->sin_addr = endpoint.address.getSysIPv4();

				mSize = Number::convert<socklen_t>(sizeof(sockaddr_in));
			}
			else if (endpoint.address.version() == IpVersion::V4)
			{
				sockaddr_in6* sockAddr6 = access_as<sockaddr_in6*>(&mData[0]);

				sockAddr6->sin6_family = AF_INET6;
				sockAddr6->sin6_port = htons(endpoint.port);
				sockAddr6->sin6_addr = endpoint.address.getSysIPv6();

				mSize = Number::convert<socklen_t>(sizeof(sockaddr_in6));
			}
		}

		SockAddr(const IpAddress& addr, Port port)
			: SockAddr( Endpoint(addr, port))
		{
		}

		const sockaddr* data() const
		{
			return access_as<sockaddr*>(&mData[0]);
		}

		socklen_t size() const
		{
			return mSize;
		}

		Endpoint toEndPoint() const
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
		}
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

	SOCKET makeSocket(int domain, int type, int protocol);

	void connectSocket(
		SOCKET socket, const struct sockaddr *addr,
		socklen_t addrlen
	);

	void bindSocket(
		SOCKET socket, const struct sockaddr *addr,
		socklen_t addrlen
	);

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