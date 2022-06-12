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

		if (IP.version() == IpVersion::V4)
		{
			mInternal->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (INVALID_SOCKET == mInternal->Socket)
			{
				disconnect();
				throw InternalSubsystemFailure();
			}

			sockaddr_in sockAddr;
			memset(&sockAddr, 0, sizeof(sockaddr_in));

			sockAddr.sin_family = AF_INET;
			sockAddr.sin_port = htons(port);
			IP.getSysAddress(&sockAddr.sin_addr);

			if (0 == ::connect(mInternal->Socket, (sockaddr*)&sockAddr, sizeof(sockaddr_in)))
			{
				mInternal->RemoteIP = IP;
				mInternal->RemotePort = port;

				struct sockaddr_in addrLocal;
				int addrLength = sizeof(sockaddr_in);
				getsockname(mInternal->Socket, (sockaddr*)&addrLocal, &addrLength);
				mInternal->LocalIP = IpAddress(&addrLocal.sin_addr);
				mInternal->LocalPort = addrLocal.sin_port;

				return;
			}
			else
			{
				disconnect();
				throw InternalSubsystemFailure();
			}
		}
		else if (IP.version() == IpVersion::V6)
		{
			throw StdExt::not_implemented("IPv6 connection support not yet implemented.");
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
		return (isConnected()) ? mInternal->RemoteIP : IpAddress();
	}

	Port TcpConnection::remotePort() const
	{
		return (isConnected()) ? mInternal->RemotePort : 0;
	}

	IpAddress TcpConnection::localIp() const
	{
		return (isConnected()) ? mInternal->LocalIP : IpAddress();
	}

	Port TcpConnection::localPort() const
	{
		return (isConnected()) ? mInternal->LocalPort : 0;
	}
}