#include <StdExt/IpComm/TcpConnection.h>
#include <StdExt/IpComm/Exceptions.h>

#include <StdExt/Number.h>

#include "TcpOpaque.h"

namespace StdExt::IpComm
{
	TcpConnection::TcpConnection() = default;
	TcpConnection::TcpConnection(TcpConnection&&) = default;

	TcpConnection::~TcpConnection()
	{
		disconnect();
	}

	TcpConnection& TcpConnection::operator=(TcpConnection&&) = default;

	void TcpConnection::connect(IpAddress IP, Port port)
	{	
		if (isConnected())
		{
			throw AlreadyConnected();
		}
		else if (false == IP.isValid())
		{
			throw InvalidIpAddress();
		}

		mInternal.reset(new TcpConnOpaque());

		int addr_family = (IP.version() == IpVersion::V4) ? AF_INET : AF_INET6;
		mInternal->Socket = socket(addr_family, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == mInternal->Socket)
		{
			disconnect();
			throw InternalSubsystemFailure();
		}

		sockaddr_in sockAddr4;
		sockaddr_in6 sockAddr6;

		memset(&sockAddr4, 0, sizeof(sockaddr_in));
		memset(&sockAddr6, 0, sizeof(sockaddr_in6));

		if (IP.version() == IpVersion::V4)
		{
			sockAddr4.sin_family = AF_INET;
			sockAddr4.sin_port = htons(port);
			sockAddr4.sin_addr = IP.getSysIPv4();
		}
		else
		{
			sockAddr6.sin6_family = AF_INET6;
			sockAddr6.sin6_port = htons(port);
			sockAddr6.sin6_addr = IP.getSysIPv6();
		}

		sockaddr* sockAddr = (IP.version() == IpVersion::V4) ? (sockaddr*)&sockAddr4 : (sockaddr*)&sockAddr6;
		int addrLength = Number::convert<int>(
			(IP.version() == IpVersion::V4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)
		);

		if (0 == ::connect(mInternal->Socket, sockAddr, addrLength))
		{
			mInternal->Remote.address = IP;
			mInternal->Remote.port = port;

			Endpoint endpoint = getSocketEndpoint(mInternal->Socket, IP.version());
			mInternal->Remote.address = endpoint.address;
			mInternal->Remote.port = endpoint.port;
		}
		else
		{
			disconnect();
			throw InternalSubsystemFailure();
		}
	}

	void TcpConnection::disconnect()
	{
		if (mInternal && INVALID_SOCKET != mInternal->Socket)
			closesocket(mInternal->Socket);

		mInternal.reset(nullptr);
	}

	bool TcpConnection::isConnected() const
	{
		return (mInternal && INVALID_SOCKET != mInternal->Socket);
	}

	void TcpConnection::readRaw(void* destination, size_t byteLength)
	{
		if (!isConnected())
		{
			throw NotConnected();
		}
		else if (0 == byteLength)
		{
			return;
		}
		else if (nullptr == destination)
		{
			throw std::invalid_argument("read buffer cannot be null.");
		}
		else
		{
			int readResult = recv(
				mInternal->Socket,
				access_as<char*>(destination),
				Number::convert<int>(byteLength),
			MSG_WAITALL
			);

			if (readResult < 0)
			{
				switch (readResult)
				{
				case WSAENOTCONN:
					disconnect();
					throw NotConnected();
				case WSAECONNABORTED:
				case WSAETIMEDOUT:
					disconnect();
					throw TimeOut();
				default:
					throw UnknownError();
				}
			}
		}
	}

	void TcpConnection::writeRaw(const void* data, size_t byteLength)
	{
		if (!isConnected())
		{
			throw NotConnected();
		}
		else if (0 == byteLength)
		{
			return;
		}
		else
		{
			if (send(mInternal->Socket, (char*)data, (int)byteLength, 0) > 0)
				return;
		}

		throw UnknownError();
	}

	bool TcpConnection::canRead(size_t numBytes)
	{
		return isConnected();
	}

	bool TcpConnection::canWrite(size_t numBytes, bool autoExpand)
	{
		return isConnected();
	}

	size_t TcpConnection::bytesAvailable() const
	{
		if (isConnected())
		{
			u_long out;
			ioctlsocket(mInternal->Socket, FIONREAD, &out);

			return out;
		}

		return 0;
	}

	IpAddress TcpConnection::remoteIp() const
	{
		return (isConnected()) ? mInternal->Remote.address : IpAddress();
	}

	Port TcpConnection::remotePort() const
	{
		return (isConnected()) ? mInternal->Remote.port : 0;
	}

	IpAddress TcpConnection::localIp() const
	{
		return (isConnected()) ? mInternal->Remote.address : IpAddress();
	}

	Port TcpConnection::localPort() const
	{
		return (isConnected()) ? mInternal->Remote.port : 0;
	}
}