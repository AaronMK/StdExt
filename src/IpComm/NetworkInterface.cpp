#include <StdExt/IpComm/NetworkInterface.h>

#include <StdExt/Buffer.h>
#include <StdExt/Number.h>
#include <StdExt/String.h>

#include <Mutex>

using namespace StdExt;
using namespace std;

#ifdef _WIN32

#	define WIN32_LEAN_AND_MEAN

#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <iphlpapi.h>

#	pragma comment (lib, "Iphlpapi.lib")
#	pragma comment (lib, "Ws2_32.lib")
#endif

namespace StdExt::IpComm
{

#ifdef _WIN32
	std::vector<NetworkInterface> NetworkInterface::allInterfaces()
	{
		std::vector<NetworkInterface> AllAdapters;

		ULONG flags = 
			GAA_FLAG_SKIP_MULTICAST |
			GAA_FLAG_SKIP_ANYCAST |
			GAA_FLAG_SKIP_DNS_SERVER;

		Buffer buffer(15 * 1024);
		ULONG bufferSize = Number::convert<ULONG>(buffer.size());
		ULONG errorCode = 0;

		errorCode = GetAdaptersAddresses(
			AF_UNSPEC, flags, nullptr,
			(PIP_ADAPTER_ADDRESSES)buffer.data(),
			&bufferSize
		);

		if (ERROR_BUFFER_OVERFLOW == errorCode)
		{
			buffer.resize(bufferSize);
			ULONG bufferSize = Number::convert<ULONG>(buffer.size());

			errorCode = GetAdaptersAddresses(
				AF_UNSPEC, flags, nullptr,
				(PIP_ADAPTER_ADDRESSES)buffer.data(),
				&bufferSize
			);
		}

		PIP_ADAPTER_ADDRESSES currentInterface = (PIP_ADAPTER_ADDRESSES)buffer.data();

		while(currentInterface)
		{
			String FriendlyName = convertString<char8_t>( WString(currentInterface->FriendlyName) );
			
			auto currentAddress = currentInterface->FirstUnicastAddress;

			while (currentAddress)
			{
				auto sockAddr = currentAddress->Address.lpSockaddr;

				NetworkInterface adapter;
				adapter.mName = FriendlyName;

				if (sockAddr->sa_family == AF_INET)
				{
					sockaddr_in* saIn = (sockaddr_in*)sockAddr;
					in_addr* addr = (in_addr*)&saIn->sin_addr;
					adapter.mIpAddr = IpAddress(*addr);
				}
				else if (sockAddr->sa_family == AF_INET6)
				{
					sockaddr_in* saIn = (sockaddr_in*)sockAddr;
					in6_addr* addr = (in6_addr*)&saIn->sin_addr;
					adapter.mIpAddr = IpAddress(*addr);
				}
				else
				{
					currentAddress = currentAddress->Next;
					continue;
				}

				String addrStr = adapter.address().toString();
				AllAdapters.push_back(move(adapter));

				currentAddress = currentAddress->Next;
			}

			currentInterface = currentInterface->Next;
		}

		return AllAdapters;
	}


#endif // _WIN32


	NetworkInterface::NetworkInterface()
	{

	}

	NetworkInterface NetworkInterface::any(IpVersion version)
	{
		NetworkInterface ret;
		ret.mIpAddr = IpAddress::any(version);
		ret.mName = U8String::literal(u8"<Any>");

		return ret;
	}

	NetworkInterface NetworkInterface::localhost()
	{
		NetworkInterface ret;
		ret.mIpAddr = IpAddress(U8String::literal(u8"127.0.0.1"));
		ret.mName = U8String::literal(u8"<Localhost>");

		return ret;
	}

	const IpAddress& NetworkInterface::address() const
	{
		return mIpAddr;
	}

	const StdExt::String& NetworkInterface::name() const
	{
		return mName;
	}
}