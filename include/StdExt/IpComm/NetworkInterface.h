#ifndef _STD_EXT_IP_COMM_NETWORK_INTERFACE_H_
#define _STD_EXT_IP_COMM_NETWORK_INTERFACE_H_

#include "IpComm.h"
#include "IpAddress.h"

#include <StdExt/String.h>

#include <vector>

namespace StdExt::IpComm
{
	class STD_EXT_EXPORT NetworkInterface
	{
	public:
		static NetworkInterface any(IpVersion version);
		static NetworkInterface localhost();

		static std::vector<NetworkInterface> allInterfaces();

		const IpAddress& address() const;
		const StdExt::String& name() const;

	private:
		NetworkInterface();

		StdExt::String mName;
		IpAddress mIpAddr;
	};
}

#endif // _STD_EXT_IP_COMM_NETWORK_INTERFACE_H_