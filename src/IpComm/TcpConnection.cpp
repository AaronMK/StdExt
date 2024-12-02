#include <StdExt/IpComm/TcpConnection.h>

#include <StdExt/IpComm/Exceptions.h>
#include <StdExt/IpComm/SockAddr.h>

#include <StdExt/Number.h>
#include <StdExt/Utility.h>

#include "IpCommOpaque.h"

#include <iostream>
#include <sstream>

using namespace std::chrono;

namespace StdExt::IpComm
{
	TcpConnection::TcpConnection()
	{
	}

	TcpConnection::~TcpConnection()
	{
		try
		{
			disconnect();
		}
		catch(...)
		{
		}
	}

	void TcpConnection::connect(IpAddress IP, Port port)
	{
		bool connected = false;

		if (isConnected())
			throw AlreadyConnected();
		else if (false == IP.isValid())
			throw InvalidIpAddress();

		try
		{
			mInternal = std::make_unique<TcpConnOpaque>();

			int addr_family = (IP.version() == IpVersion::V4) ? AF_INET : AF_INET6;
			mInternal->Socket = makeSocket(addr_family, SOCK_STREAM, IPPROTO_TCP);
		}
		catch(const std::exception&)
		{
			mInternal.reset();
			throw;
		}

		try
		{
			setSocketBlocking(mInternal->Socket, true);

			SockAddr sock_addr( Endpoint(IP, port) );
			connectSocket(mInternal->Socket, sock_addr.data(), sock_addr.size());

			connected = true;

			mInternal->Remote.address = IP;
			mInternal->Remote.port = port;

			mInternal->Local = getSocketEndpoint(mInternal->Socket);

			setReceiveTimeout(mReceiveTimeout);
		}
		catch(const std::exception&)
		{
			try
			{
				if ( connected )
					shutdownSocket(mInternal->Socket);
			}
			catch(...)
			{
			}

			closeSocket(mInternal->Socket);

			mInternal.reset();
			throw;
		}
	}

	void TcpConnection::disconnect()
	{
		if ( !mInternal )
			return;

		auto final_action = finalBlock(
			[&]()
			{
				try
				{
					closeSocket(mInternal->Socket);
				}
				catch(...)
				{
				}

				mInternal.reset();
				SocketStream::clear();
			}
		);

		shutdownSocket(mInternal->Socket);
	}

	bool TcpConnection::isConnected() const
	{
		return (mInternal && INVALID_SOCKET != mInternal->Socket);
	}

	void TcpConnection::readRaw(void* destination, size_t byteLength)
	{
		if ( SocketStream::bytesAvailable() >= byteLength)
		{
			SocketStream::readRaw(destination, byteLength);
			return;
		}

		if ( !isConnected() )
			throw NotConnected();

		if ( nullptr == destination )
			throw std::invalid_argument("Read buffer cannot be null.");

		size_t bytes_needed  = byteLength - SocketStream::bytesAvailable();
		size_t bytes_to_read = std::max(bytes_needed, socketBytesAvailable());

		try
		{
			write(
				bytes_to_read, [&](void* dest, size_t dest_size)
				{
					return recvSocket(
						mInternal->Socket, dest,
						dest_size, MSG_WAITALL
					);
				}
			);
		}
		catch(const NotConnected&)
		{
			disconnect();
			throw;
		}

		if ( SocketStream::bytesAvailable() >= byteLength )
			SocketStream::readRaw(destination, byteLength);
		else
			throw TimeOut();
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
			sendSocket(mInternal->Socket, data, byteLength);
		}
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
		return SocketStream::bytesAvailable() + socketBytesAvailable();
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

	void TcpConnection::setReceiveTimeout(const microseconds& timeout_period)
	{
		if (timeout_period.count() == 0 && mReceiveTimeout.count() == 0 )
			return;

		mReceiveTimeout = timeout_period;
		
		if ( isConnected() )
		{
			#ifdef STD_EXT_WIN32
			DWORD ms_timeout = 
				Number::convert<DWORD>(
					duration_cast<milliseconds>(timeout_period).count()
				);
			setsockopt(
				mInternal->Socket, SOL_SOCKET, SO_RCVTIMEO,
				access_as<const char*>(&ms_timeout), sizeof(ms_timeout)
			);
			#else
			auto secs = duration_cast<seconds>(timeout_period);

			timeval time_out;
			time_out.tv_sec  = secs.count();
			time_out.tv_usec = Number::convert<decltype(time_out.tv_usec)>((timeout_period - secs).count());

			auto result = setsockopt(
				mInternal->Socket, SOL_SOCKET, SO_RCVTIMEO,
				&time_out, sizeof(time_out)
			);

			if ( 0 != result )
				throwDefaultError(getLastError());

			#endif
		}
	}

	size_t TcpConnection::socketBytesAvailable() const
	{
		if (isConnected())
		{
			#ifdef _WIN32
			u_long out;
			ioctlsocket(mInternal->Socket, FIONREAD, &out);
			return out;
			#else
			int out;
			ioctl(mInternal->Socket, FIONREAD, &out);
			return Number::convert<size_t>(out);
			#endif
		}

		return 0;
	}

	TcpConnOpaque::TcpConnOpaque()
	{
		Socket = INVALID_SOCKET;
	}

	TcpConnOpaque::~TcpConnOpaque()
	{
	}
}
