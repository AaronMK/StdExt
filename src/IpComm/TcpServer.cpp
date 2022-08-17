#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/Exceptions.h>

#include "TcpOpaque.h"

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

		sockaddr_in inAddr;
		int addrLength = sizeof(sockaddr_in);

		SOCKET acceptSocket = accept(mInternal->Socket, (sockaddr*)&inAddr, &addrLength);

		if (INVALID_SOCKET == acceptSocket)
		{
			switch (WSAGetLastError())
			{
			case WSAEINVAL:
				throw NotListening();
				break;
			case WSAECONNRESET:
				throw ConnectionReset();
				break;
			default:
				throw InternalSubsystemFailure();
				break;
			}
		}

		std::unique_ptr<TcpConnOpaque> ptrRet(new TcpConnOpaque());
		ptrRet->Socket = acceptSocket;
		ptrRet->RemoteIP = IpAddress(inAddr.sin_addr);
		ptrRet->RemotePort = inAddr.sin_port;

		struct sockaddr_in addrLocal;
		getsockname(ptrRet->Socket, (sockaddr*)&addrLocal, &addrLength);
		ptrRet->LocalIP = IpAddress(addrLocal.sin_addr);
		ptrRet->LocalPort = mInternal->ListenPort;

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

		mInternal.reset(new TcpServerOpaque());

		if (addr.version() == IpVersion::V4)
		{
			mInternal->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (INVALID_SOCKET == mInternal->Socket)
				throw InternalSubsystemFailure();

			sockaddr_in sockAddr;
			memset(&sockAddr, 0, sizeof(sockaddr_in));

			sockAddr.sin_family = AF_INET;
			sockAddr.sin_port = htons(port);
			sockAddr.sin_addr = addr.getSysIPv4();

			if (0 == ::bind(mInternal->Socket, (sockaddr*)&sockAddr, sizeof(sockaddr_in)) &&
				SOCKET_ERROR != listen(mInternal->Socket, SOMAXCONN))
			{
				mInternal->ListenIP = addr;
				mInternal->ListenPort = port;

				return;
			}
			else
			{
				closesocket(mInternal->Socket);
				mInternal.reset(nullptr);

				throw InternalSubsystemFailure();
			}
		}
		else if (addr.version() == IpVersion::V6)
		{
			throw StdExt::not_implemented("IPv6 server support not yet implemeneted.");
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
			closesocket(mInternal->Socket);
			mInternal.reset(nullptr);
		}
	}
}