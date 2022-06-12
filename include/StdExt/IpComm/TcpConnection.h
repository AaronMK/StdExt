#ifndef _STD_EXT_IP_COMM_H_TCP_CONNECTION_H_
#define _STD_EXT_IP_COMM_H_TCP_CONNECTION_H_

#include "../Streams/ByteStream.h"

#include "IpAddress.h"

#include <memory>

namespace StdExt::IpComm
{
	struct TcpConnOpaque;

	class STD_EXT_EXPORT TcpConnection : public Streams::ByteStream
	{
		friend class TcpServer;

	public:
		TcpConnection(const TcpConnection&) = delete;
		TcpConnection& operator=(const TcpConnection&) = delete;

		TcpConnection();
		TcpConnection(TcpConnection&&);

		virtual ~TcpConnection();

		TcpConnection& operator=(TcpConnection&&);

		/**
		 * @brief
		 *  Attempts to connect to the passed IP and port, blocking
		 *  until success or failure is determined.
		 */
		void connect(IpAddress remote_ip, Port remote_port);

		/**
		 * @brief
		 *  If connected, closes the connection.
		 */
		void disconnect();

		/**
		 * @brief
		 *  Returns true if connected.
		 */
		bool isConnected() const;

		/**
		 * @brief
		 *  Gets the remote IP Address of the connection.
		 */
		IpAddress remoteIp() const;

		/**
		 * @brief
		 *  Gets the remote port of the connection.
		 */
		Port remotePort() const;

		/**
		 * @brief
		 *  Gets the local IP Address.  When accepted by a TcpServer
		 *  this will be the listening IP of the server that accepted
		 *  the connection.
		 */
		IpAddress localIp() const;

		/**
		 * @brief
		 *  Gets the local port.  When accepted by a TcpServer
		 *  this will be the listening port of the server that accepted
		 *  the connection.
		 */
		Port localPort() const;


		void readRaw(void* destination, size_t byteLength) override;
		void writeRaw(const void* data, size_t byteLength) override;

		size_t bytesAvailable() const override;

	private:
		TcpConnection(std::unique_ptr<TcpConnOpaque>&& opaque);

		std::unique_ptr<TcpConnOpaque> mInternal;
	};
}

#endif // !_STD_EXT_IP_COMM_H_