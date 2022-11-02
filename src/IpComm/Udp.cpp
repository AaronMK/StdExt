#include <StdExt/IpComm/Udp.h>
#include <StdExt/IpComm/SockAddr.h>

#include <StdExt/IpComm/Exceptions.h>

#include "IpCommOpaque.h"

namespace StdExt::IpComm
{
	struct UdpServerOpaque
	{
		WsaHandle Handle;
		SOCKET Socket = INVALID_SOCKET;

		Endpoint Local{};
	};

	UdpServer::UdpServer()
	{
	}
	
	UdpServer::~UdpServer()
	{
		close();
	}

	void UdpServer::bind(IpVersion version)
	{
		return bind(IpAddress::any(version), 0);
	}

	void UdpServer::bind(Port port, IpVersion version)
	{
		return bind(IpAddress::any(version), port);
	}
	
	void UdpServer::bind(IpAddress addr, Port port)
	{
		if (isListening())
			throw AlreadyConnected();

		try
		{
			mInternal.reset(new UdpServerOpaque());

			int addr_family = (addr.version() == IpVersion::V4) ? AF_INET : AF_INET6;
			mInternal->Socket = makeSocket(addr_family, SOCK_DGRAM, IPPROTO_UDP);

			SockAddr sock_addr(addr, port);

			bindSocket(mInternal->Socket, sock_addr.data(), sock_addr.size());

			int broadcast = 1;
			auto result = setsockopt(
				mInternal->Socket, SOL_SOCKET, SO_BROADCAST,
				access_as<const char*>(&broadcast), sizeof(broadcast)
			);

			mInternal->Local = getSocketEndpoint(mInternal->Socket);
		}
		catch (const std::exception&)
		{
			close();
			throw;
		}
	}
	
	void UdpServer::close()
	{
		if (isListening())
		{
			#ifdef _WIN32
			closesocket(mInternal->Socket);
			#else
			shutdown(mInternal->Socket, SHUT_RDWR);
			close(mInternal->Socket);
			#endif
			mInternal.reset(nullptr);
		}
	}

	bool UdpServer::isListening() const
	{
		return (nullptr != mInternal.get());
	}

	void UdpServer::sendPacket(void* data, size_t size, Endpoint dest)
	{	
		if (isListening())
		{
			SockAddr sock_addr(dest);
			
			sendTo(
				mInternal->Socket, static_cast<char*>(data),
				size, sock_addr.data(), sock_addr.size()
			);
		}
		else
		{
			throw NotConnected();
		}
	}
	
	size_t UdpServer::receivePacket(void* data, size_t size, Endpoint* source)
	{
		if (isListening())
		{
			SockAddr remote_addr;

			size_t count = receiveFrom(
				mInternal->Socket, data, size, remote_addr.data(),
				remote_addr.sizeInOut()
			);

			if ( source )
				*source = remote_addr.toEndpoint();

			return count;
		}
		else
		{
			throw NotConnected();
		}
	}

	Endpoint UdpServer::localEndpoint() const
	{
		return ( nullptr == mInternal.get() ) ?
			Endpoint() : mInternal->Local;
	}
}