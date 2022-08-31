#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/Exceptions.h>

#include "IpCommOpaque.h"

namespace StdExt::IpComm
{
	TcpServer::TcpServer() = default;
	TcpServer::TcpServer(TcpServer&&) = default;

	TcpServer::~TcpServer()
	{
		disconnect();
	}

	TcpServer& TcpServer::operator=(TcpServer&&) = default;

	TcpConnection TcpServer::getClient()
	{
		if (false == isListening())
		{
			throw NotListening();
		}

		sockaddr_in sockAddr4;
		sockaddr_in6 sockAddr6;

		memset(&sockAddr4, 0, sizeof(sockaddr_in));
		memset(&sockAddr6, 0, sizeof(sockaddr_in6));

		const Endpoint& local_end_point = mInternal->LocalEndpoint;
		const IpVersion local_ip_version = local_end_point.address.version();
		
		sockaddr* sockAddr = (local_ip_version == IpVersion::V4) ? (sockaddr*)&sockAddr4 : (sockaddr*)&sockAddr6;
		socklen_t addrLength = Number::convert<int>(
			(local_ip_version == IpVersion::V4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)
		);

		SOCKET acceptSocket = accept(mInternal->Socket, sockAddr, &addrLength);

		if (INVALID_SOCKET == acceptSocket)
		{
			switch (getLastError())
			{
			case EINVAL:
				throw NotListening();
				break;
			case ECONNRESET:
				throw ConnectionReset();
				break;
			default:
				throw InternalSubsystemFailure();
				break;
			}
		}

		std::unique_ptr<TcpConnOpaque> ptrRet(new TcpConnOpaque());
		ptrRet->Socket = acceptSocket;

		ptrRet->Remote = ( local_ip_version == IpVersion::V4 ) ?
			Endpoint(sockAddr4.sin_addr, htons(sockAddr4.sin_port)) :
			Endpoint(sockAddr6.sin6_addr, htons(sockAddr6.sin6_port));

		ptrRet->Local = getSocketEndpoint(ptrRet->Socket, mInternal->LocalEndpoint.address.version());

		TcpConnection ret;
		ret.mInternal = std::move(ptrRet);

		return ret;
	}

	void TcpServer::bind(Port port, IpVersion version)
	{
		return bind(IpAddress::any(version), port);
	}

	void TcpServer::bind(IpAddress addr, Port port)
	{
		if (isListening())
			throw AlreadyConnected();

		if (false == addr.isValid())
			throw InvalidIpAddress();

		try
		{
			mInternal.reset(new TcpServerOpaque());

			int addr_family = (addr.version() == IpVersion::V4) ? AF_INET : AF_INET6;
			mInternal->Socket = makeSocket(addr_family, SOCK_STREAM, IPPROTO_TCP);

			sockaddr_in sockAddr4;
			sockaddr_in6 sockAddr6;

			memset(&sockAddr4, 0, sizeof(sockaddr_in));
			memset(&sockAddr6, 0, sizeof(sockaddr_in6));

			sockaddr* sockAddr = (addr.version() == IpVersion::V4) ? (sockaddr*)&sockAddr4 : (sockaddr*)&sockAddr6;
			int addrLength = Number::convert<int>(
				(addr.version() == IpVersion::V4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)
			);

			if (addr.version() == IpVersion::V4)
			{
				sockAddr4.sin_family = AF_INET;
				sockAddr4.sin_port = htons(port);
				sockAddr4.sin_addr = addr.getSysIPv4();
			}
			else
			{
				sockAddr6.sin6_family = AF_INET6;
				sockAddr6.sin6_port = htons(port);
				sockAddr6.sin6_addr = addr.getSysIPv6();
			}

			bindSocket(mInternal->Socket, sockAddr, addrLength);
			listenSocket(mInternal->Socket, SOMAXCONN);

			mInternal->LocalEndpoint = Endpoint(addr, port);
			return;
		}
		catch (const std::exception&)
		{
			disconnect();
			throw;
		}
	}

	bool TcpServer::isListening() const
	{
		return (mInternal && INVALID_SOCKET != mInternal->Socket);
	}

	void TcpServer::disconnect()
	{
		if (isListening())
		{
			#ifdef _WIN32
			closesocket(mInternal->Socket);
			#else
			close(mInternal->Socket);
			#endif
			mInternal.reset(nullptr);
		}
	}
}