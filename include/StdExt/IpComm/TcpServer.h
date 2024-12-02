#ifndef _STD_EXT_IP_COMM_TCP_SERVER_H_
#define _STD_EXT_IP_COMM_TCP_SERVER_H_

#include "TcpConnection.h"

#include <memory>

namespace StdExt::IpComm
{
	struct TcpServerOpaque;

	class STD_EXT_EXPORT TcpServer
	{
	public:
		TcpServer(const TcpServer&) = delete;
		TcpServer& operator=(const TcpServer&) = delete;

		TcpServer();
		TcpServer(TcpServer&&);

		virtual ~TcpServer();

		TcpServer& operator=(TcpServer&&);

		/**
		 * @brief
		 *  Blocks until a connection request is made to the server.  If the server
		 *  is not listening, or is disconnected, an appropriate exception will be thrown.
		 */
		TcpConnection getClient();

		/**
		 * @brief
		 *  Binds the server to the passed port on all IP Interfaces with the
		 *  passed version on the system.
		 */
		void bind(Port port, IpVersion version);

		/**
		 * @brief
		 *  Binds the server to the passed port on a specific Ip interface
		 *  of the system.
		 */
		void bind(IpAddress addr, Port port);

		/**
		 * @brief
		 *  Stops the server and frees the IP/Port combination.
		 */
		void disconnect();

		/**
		 * @brief
		 *  Returns true if the server is listening.
		 */
		bool isListening() const;

	private:
		std::shared_ptr<TcpServerOpaque> mInternal;
	};
}

#endif //!_STD_EXT_IP_COMM_TCP_SERVER_H_