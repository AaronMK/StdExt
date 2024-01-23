#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/Exceptions.h>
#include <StdExt/IpComm/SockAddr.h>

#include "IpCommOpaque.h"

#include <iostream>

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
			throw NotListening();

		SockAddr remote_addr;
		SOCKET sock = acceptSocket(mInternal->Socket, remote_addr.data(), remote_addr.sizeInOut());

		auto ptrRet = std::make_unique<TcpConnOpaque>();

		ptrRet->Socket = sock;
		ptrRet->Remote = remote_addr.toEndpoint();
		ptrRet->Local  = getSocketEndpoint(ptrRet->Socket);

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
			mInternal = std::make_shared<TcpServerOpaque>(addr, port);
		}
		catch (const std::exception&)
		{
			disconnect();
			throw;
		}
	}

	bool TcpServer::isListening() const
	{
		return (nullptr != mInternal.get());
	}

	void TcpServer::disconnect()
	{
		mInternal.reset();
	}

	/////////////////////////////////////////////

	TcpServerOpaque::TcpServerOpaque(IpAddress addr, Port port)
	{
		int addr_family = (addr.version() == IpVersion::V4) ? AF_INET : AF_INET6;
		Socket = INVALID_SOCKET;

		try
		{
			Socket = makeSocket(addr_family, SOCK_STREAM, IPPROTO_TCP);
		}
		catch(const std::exception&)
		{
			Socket = INVALID_SOCKET;
			throw;
		}

		SockAddr sock_addr( Endpoint(addr, port) );

		try
		{
			#ifdef STD_EXT_WIN32
			setSocketOption<BOOL>(Socket, SOL_SOCKET, SO_REUSEADDR, FALSE);
			#else
			linger linger_spec;
			linger_spec.l_onoff = 1;
			linger_spec.l_linger = 1;

			setSocketOption<linger>(Socket, SOL_SOCKET, SO_LINGER, linger_spec);
			#endif

			bindSocket(Socket, sock_addr.data(), sock_addr.size());
			listenSocket(Socket, SOMAXCONN);
		}
		catch(const std::exception&)
		{
			#ifdef STD_EXT_WIN32
			closesocket(Socket);
			#else
			close(Socket);
			#endif

			Socket = INVALID_SOCKET;

			throw;
		}
		
		LocalEndpoint = Endpoint(addr, port);
	}

	TcpServerOpaque::~TcpServerOpaque()
	{
		if ( INVALID_SOCKET == Socket )
			return;

		try
		{
			#ifdef STD_EXT_WIN32
			auto result  = closesocket(Socket);
			bool success = (SOCKET_ERROR != result);
			#else
			auto result  = close(Socket);
			bool success = ( 0 == result );
			#endif

			if ( !success )
				throwDefaultError(getLastError());
		}
		catch(const std::exception& e)
		{
			std::cerr << "Socket Destruction Exception: " << e.what() << std::endl;
		}
	}
}