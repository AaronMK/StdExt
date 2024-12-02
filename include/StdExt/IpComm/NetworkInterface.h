#ifndef _STD_EXT_IP_COMM_NETWORK_INTERFACE_H_
#define _STD_EXT_IP_COMM_NETWORK_INTERFACE_H_

#include "IpComm.h"
#include "IpAddress.h"

#include <StdExt/String.h>

#include <vector>

namespace StdExt::IpComm
{
	/**
	 * @brief
	 *  A network interface representing an IP Address assigned to the local host.
	 */
	class STD_EXT_EXPORT NetworkInterface
	{
	public:
		static NetworkInterface any(IpVersion version);
		static NetworkInterface localhost(IpVersion version);

		static std::vector<NetworkInterface> allInterfaces();

		const IpAddress& address() const;
		const StdExt::String& name() const;

		/**
		 * @brief
		 *  Returns true if the interface is temporary.
		 */
		bool isTemporary() const;

		/**
		 * @brief
		 *  Gets the length of the network prefix in bits.
		 */
		uint8_t prefixLength() const;

	private:
		NetworkInterface();

		IpAddress mIpAddr;
		StdExt::String mName;
		uint8_t mPrefixLength;
		bool mIsTemporary;
	};
}

#endif // _STD_EXT_IP_COMM_NETWORK_INTERFACE_H_