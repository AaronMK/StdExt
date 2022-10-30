#ifndef _STD_EXT_IP_COMM_UDP_H_
#define _STD_EXT_IP_COMM_UDP_H_

#include "IpAddress.h"

#include <memory>

namespace StdExt::IpComm
{
	struct UdpServerOpaque;
	
	class STD_EXT_EXPORT UdpServer
	{
	private:
		std::unique_ptr<UdpServerOpaque> mInternal;

	public:
		UdpServer();
		virtual ~UdpServer();

		void listen(Port port, IpVersion version);
		void listen(IpAddress addr, Port port);

		void close();

		bool isListening() const;
		Endpoint localEndpoint() const;

		void sendPacket(void* data, size_t size, Endpoint dest);
		size_t receivePacket(void* data, size_t size, Endpoint* source = nullptr);
	};
}

#endif // !_STD_EXT_IP_COMM_UDP_H_