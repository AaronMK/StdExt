#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/Exceptions.h>
#include <StdExt/IpComm/SockAddr.h>

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

		SockAddr remote_addr;
		SOCKET acceptSocket = accept(mInternal->Socket, remote_addr.data(), remote_addr.sizeInOut());

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
		ptrRet->Remote = remote_addr.toEndpoint();
		ptrRet->Local = getSocketEndpoint(ptrRet->Socket);

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

			SockAddr sock_addr( Endpoint(addr, port) );

			bindSocket(mInternal->Socket, sock_addr.data(), sock_addr.size() );
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
			shutdown(mInternal->Socket, SHUT_RDWR);
			close(mInternal->Socket);
			#endif
			mInternal.reset(nullptr);
		}
	}
}