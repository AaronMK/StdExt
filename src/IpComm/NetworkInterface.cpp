#include <StdExt/IpComm/NetworkInterface.h>
#include <StdExt/IpComm/SockAddr.h>
#include <StdExt/IpComm/Endpoint.h>

#include <StdExt/Memory/Endianess.h>

#include <StdExt/Buffer.h>
#include <StdExt/Number.h>
#include <StdExt/String.h>

#include <bit>

using namespace StdExt;
using namespace std;

#ifdef STD_EXT_WIN32

#	define WIN32_LEAN_AND_MEAN

#	include <windows.h>
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <iphlpapi.h>

#	pragma comment (lib, "Iphlpapi.lib")
#	pragma comment (lib, "Ws2_32.lib")
#else
#	include <StdExt/Utility.h>

#	include <sys/types.h>
#	include <ifaddrs.h>
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
				auto lp_sockAddr = currentAddress->Address.lpSockaddr;

				if ( lp_sockAddr )
				{
					SockAddr sock_addr(lp_sockAddr);

					NetworkInterface adapter;
					adapter.mIsTemporary = (currentAddress->SuffixOrigin == NlsoRandom);
					adapter.mPrefixLength = Number::convert<uint8_t>(currentAddress->OnLinkPrefixLength);
					adapter.mName = FriendlyName;
					adapter.mIpAddr = sock_addr.toEndpoint().address;

					AllAdapters.push_back(move(adapter));
				}

				currentAddress = currentAddress->Next;
			}

			currentInterface = currentInterface->Next;
		}

		return AllAdapters;
	}
#else
	
	std::vector<NetworkInterface> NetworkInterface::allInterfaces()
	{
		std::vector<NetworkInterface> result;
		ifaddrs* iface_begin = nullptr;

		getifaddrs(&iface_begin);

		if ( nullptr == iface_begin )
			return result;

		auto cleanup = finalBlock(
			[&]()
			{
				freeifaddrs(iface_begin);
			}
		);

		for (ifaddrs* iface = iface_begin; nullptr != iface; iface = iface->ifa_next)
		{
			SockAddr  sock_addr(iface->ifa_addr);
			IpAddress ip(sock_addr.toEndpoint().address);

			if ( !ip.isValid() || ip.isLoopback() )
				continue;

			NetworkInterface net_iface;
			net_iface.mName = convertString<char8_t>( CString(iface->ifa_name) );
			net_iface.mIpAddr = ip;

			if ( iface->ifa_netmask )
			{
				auto mask_ip = SockAddr(iface->ifa_netmask).toEndpoint().address;
				auto octets = mask_ip.getOctets();

				if ( octets.size() == 16 )
				{
					auto msb = from_big_endian(*access_as<uint64_t*>(&octets[0]));
					auto msb_width = std::countl_one(msb);

					if ( msb_width == 64 )
					{
						auto lsb = from_big_endian(*access_as<uint64_t*>(&octets[8]));
						auto lsb_width = std::countl_one(lsb);
						net_iface.mPrefixLength = msb_width + lsb_width;
					}
					else
					{
						net_iface.mPrefixLength = msb_width;
					}
				}
				else if ( octets.size() == 4 )
				{
					uint32_t iaddr = from_big_endian(*access_as<uint32_t*>(&octets[0]));
					net_iface.mPrefixLength = std::countl_one(iaddr);
				}
			}

			result.push_back( std::move(net_iface) );
		}

		return result;
	}

#endif // _WIN32


	NetworkInterface::NetworkInterface()
	{
		mIsTemporary = false;
		mPrefixLength = 0;
		mIsTemporary = false;
	}

	NetworkInterface NetworkInterface::any(IpVersion version)
	{
		NetworkInterface ret;
		ret.mIpAddr = IpAddress::any(version);
		ret.mName = U8String::literal(u8"<Any>");
		ret.mPrefixLength = ( IpVersion::V4 == version ) ? 32 : 128;
		ret.mIsTemporary = false;

		return ret;
	}

	NetworkInterface NetworkInterface::localhost(IpVersion version)
	{
		NetworkInterface ret;
		ret.mIpAddr = IpAddress::loopback(version);
		ret.mName = U8String::literal(u8"<Localhost>");
		ret.mPrefixLength = ( IpVersion::V4 == version ) ? 32 : 128;
		ret.mIsTemporary = false;

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
	
	bool NetworkInterface::isTemporary() const
	{
		return mIsTemporary;
	}

	uint8_t NetworkInterface::prefixLength() const
	{
		return mPrefixLength;
	}
}