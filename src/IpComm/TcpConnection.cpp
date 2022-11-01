#include <StdExt/IpComm/TcpConnection.h>
#include <StdExt/IpComm/Exceptions.h>

#include <StdExt/IpComm/SockAddr.h>

#include <StdExt/Number.h>

#include "IpCommOpaque.h"

using namespace std::chrono;

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

		try
		{
			mInternal.reset(new TcpConnOpaque());

			int addr_family = (IP.version() == IpVersion::V4) ? AF_INET : AF_INET6;
			mInternal->Socket = makeSocket(addr_family, SOCK_STREAM, IPPROTO_TCP);

			if (INVALID_SOCKET == mInternal->Socket)
			{
				disconnect();
				throw InternalSubsystemFailure();
			}

			SockAddr sock_addr( Endpoint(IP, port) );
			connectSocket(mInternal->Socket, sock_addr.data(), sock_addr.size() );

			mInternal->Remote.address = IP;
			mInternal->Remote.port = port;

			Endpoint endpoint = getSocketEndpoint(mInternal->Socket, IP.version());
			mInternal->Remote.address = endpoint.address;
			mInternal->Remote.port = endpoint.port;

			setReceiveTimeout(mReceiveTimeout);
		}
		catch(const std::exception&)
		{
			disconnect();
			throw;
		}
	}

	void TcpConnection::disconnect()
	{
		if (mInternal && INVALID_SOCKET != mInternal->Socket)
		{
			#ifdef _WIN32
			closesocket(mInternal->Socket);
			#else
			shutdown(mInternal->Socket, SHUT_RDWR);
			close(mInternal->Socket);
			#endif
		}

		mInternal.reset(nullptr);
		SocketStream::clear();
	}

	bool TcpConnection::isConnected() const
	{
		return (mInternal && INVALID_SOCKET != mInternal->Socket);
	}

	size_t TcpConnection::receive(void* destination, size_t byteLength)
	{	
		if (!isConnected())
		{
			throw NotConnected();
		}
		else if (0 == byteLength)
		{
			return 0;
		}
		else if (nullptr == destination)
		{
			throw std::invalid_argument("read buffer cannot be null.");
		}
		else
		{
			try
			{
				size_t base_bytes = std::min(SocketStream::bytesAvailable(), byteLength);
				size_t socket_bytes_to_read = 0;
				size_t socket_bytes_read = 0;
				
				// Do the socket read first since that can possibly throw and exception.
				// if it does, nothing is read and the base retains its cached data.
				if (base_bytes < byteLength)
				{
					socket_bytes_to_read = byteLength - base_bytes;
					void* socket_destination = access_as<std::byte*>(destination) + base_bytes;

					socket_bytes_read = recvSocket(
						mInternal->Socket, socket_destination,
						socket_bytes_to_read, 0
					);
				}

				SocketStream::readRaw(destination, base_bytes);

				return base_bytes + socket_bytes_read;
			}
			catch (const NotConnected&)
			{
				disconnect();
				throw;
			}
		}
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
			throw std::invalid_argument("Read buffer cannot be null.");
		}
		else
		{
			try
			{
				size_t base_bytes = std::min(SocketStream::bytesAvailable(), byteLength);

				if (base_bytes < byteLength)
				{
					size_t socket_bytes_to_read = byteLength - base_bytes;
					void* socket_destination = access_as<std::byte*>(destination) + base_bytes;
					
					size_t socket_bytes_read = recvSocket(
						mInternal->Socket, socket_destination,
						socket_bytes_to_read, MSG_WAITALL
					);

					if (socket_bytes_read < socket_bytes_to_read)
					{
						SocketStream::writeRaw(socket_destination, socket_bytes_read);
						throw TimeOut();
					}
				}

				SocketStream::readRaw(destination, base_bytes);
			}
			catch (const NotConnected&)
			{
				disconnect();
				throw;
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

		throw unknown_error();
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
		return SocketStream::bytesAvailable() + sysBytesAvailable();
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
			#ifdef _WIN32
			DWORD ms_timeout = 
				Number::convert<DWORD>(
					duration_cast<milliseconds>(timeout_period).count()
				);
			setsockopt(
				mInternal->Socket, SOL_SOCKET, SO_RCVTIMEO,
				access_as<const char*>(&ms_timeout), sizeof(ms_timeout)
			);
			#else
			throw not_implemented();
			#endif
		}
	}

	size_t TcpConnection::sysBytesAvailable() const
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
}