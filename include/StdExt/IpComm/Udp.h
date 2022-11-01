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

		void bind(IpVersion version);
		void bind(Port port, IpVersion version);
		void bind(IpAddress addr, Port port);

		void close();

		bool isListening() const;
		Endpoint localEndpoint() const;

		void sendPacket(void* data, size_t size, Endpoint dest);
		size_t receivePacket(void* data, size_t size, Endpoint* source = nullptr);
	};
}

#endif // !_STD_EXT_IP_COMM_UDP_H_