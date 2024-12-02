#ifndef _STD_EXT_IP_COMM_UDP_H_
#define _STD_EXT_IP_COMM_UDP_H_

#include "IpAddress.h"

#include <memory>

namespace StdExt::IpComm
{
	struct UdpOpaque;
	
	/**
	 * @brief
	 *  UDP communication class.
	 */
	class STD_EXT_EXPORT Udp
	{
	private:
		std::unique_ptr<UdpOpaque> mInternal;

	public:
		Udp();
		virtual ~Udp();

		void bind(IpVersion version);
		void bind(Port port, IpVersion version);
		void bind(const IpAddress& addr, Port port);

		void close();

		bool isListening() const;
		Endpoint localEndpoint() const;

		void sendPacket(void* data, size_t size, const Endpoint& dest);
		size_t receivePacket(void* data, size_t size, Endpoint* source = nullptr);
	};
}

#endif // !_STD_EXT_IP_COMM_UDP_H_
