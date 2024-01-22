#ifndef _STD_EXT_IP_COMM_TCP_CONNECTION_H_
#define _STD_EXT_IP_COMM_TCP_CONNECTION_H_

#include "../Streams/SocketStream.h"

#include "IpAddress.h"

#include <chrono>
#include <memory>

namespace StdExt::IpComm
{
	struct TcpConnOpaque;

	class STD_EXT_EXPORT TcpConnection : public Streams::SocketStream
	{
		friend class TcpServer;

	public:
		TcpConnection(const TcpConnection&) = delete;
		TcpConnection(TcpConnection&&) = default;

		TcpConnection& operator=(const TcpConnection&) = delete;
 		TcpConnection& operator=(TcpConnection&&) = default;

		TcpConnection();

		virtual ~TcpConnection();

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

		/**
		 * @brief
		 *  Sets the receive timeout on the connection.
		 */
		void setReceiveTimeout(const std::chrono::microseconds& timeout_period);

		void readRaw(void* destination, size_t byteLength) override;
		void writeRaw(const void* data, size_t byteLength) override;
		bool canRead(size_t numBytes) override;
		bool canWrite(size_t numBytes, bool autoExpand = false) override;

		size_t bytesAvailable() const override;

	private:
		TcpConnection(std::unique_ptr<TcpConnOpaque>&& opaque);

		/**
		 * @brief
		 *  Gets the number of bytes currently available to be read from the socket.
		 */
		size_t socketBytesAvailable() const;

		std::unique_ptr<TcpConnOpaque> mInternal;

		/**
		 * @brief
		 *  Timeout on receive calls for the connection in the units the
		 *  target platform expects.
		 */
		std::chrono::microseconds mReceiveTimeout{0};
	};
}

#endif // !_STD_EXT_IP_COMM_TCP_CONNECTION_H_