#include <StdExt/IpComm/IpAddress.h>

#include <StdExt/Serialize/Exceptions.h>


#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <ws2tcpip.h>
#include <In6addr.h>

#pragma comment(lib, "ws2_32.lib")

namespace StdExt::IpComm
{
	IpAddress IpAddress::any(IpVersion version)
	{
		if (IpVersion::V4 == version)
		{
			in_addr winAddr;
			memset(&winAddr, 0, sizeof(in_addr));

			winAddr.S_un.S_addr = INADDR_ANY;

			return IpAddress(&winAddr);
		}
		else if (IpVersion::V6 == version)
		{
			return IpAddress(&in6addr_any);
		}

		return IpAddress();
	}

	IpAddress::IpAddress()
	{
		static_assert( sizeof(IpAddress::mData) >= sizeof(in_addr) &&
		               sizeof(IpAddress::mData) >= sizeof(in6_addr),
		               "Not enough local data to store system IpAddress data.");

		memset(mData, 0, sizeof(mData));
		mVersion = IpVersion::NONE;
	}
	
	IpAddress::IpAddress(const char* addr)
		: IpAddress()
	{
		IpAddress addrTest(addr, IpVersion::V4);

		if (addrTest.isValid())
		{
			*this = addrTest;
			return;
		}

		addrTest = IpAddress(addr, IpVersion::V6);

		if (addrTest.isValid())
		{
			*this = addrTest;
			return;
		}
	}

	IpAddress::IpAddress(const std::string& addr)
		: IpAddress(addr.c_str())
	{
	}

	IpAddress::IpAddress(const StdExt::String& addr)
		: IpAddress(addr.getNullTerminated().data())
	{
	}

	IpAddress::IpAddress(const char* addr, IpVersion version)
	{
		memset(mData, 0, sizeof(mData));
		mVersion = IpVersion::NONE;

		if (IpVersion::V4 == version)
		{
			in_addr* addrPtr = reinterpret_cast<in_addr*>(&mData[0]);
			if ( 1 == InetPtonA(AF_INET, addr, addrPtr) )
			{
				mVersion = IpVersion::V4;
			}
		}
		else if (IpVersion::V6 == version)
		{
			in6_addr* addrPtr = reinterpret_cast<in6_addr*>(&mData[0]);
			if ( 1 == InetPtonA(AF_INET6, addr, addrPtr) )
			{
				mVersion = IpVersion::V6;
			}
		}
	}
	
	IpAddress::IpAddress(const std::string& addr, IpVersion version)
		: IpAddress(addr.c_str(), version)
	{
	}

	IpAddress::IpAddress(const StdExt::String& addr, IpVersion version)
		: IpAddress(addr.getNullTerminated().data(), version)
	{
	}

	IpAddress::IpAddress(const in_addr* addr)
	{
		mVersion = IpVersion::V4;
		memcpy(&mData[0], addr, sizeof(in_addr));
	}

	IpAddress::IpAddress(const in6_addr* addr)
	{
		mVersion = IpVersion::V6;
		memcpy(&mData[0], addr, sizeof(in6_addr));
	}

	IpAddress& IpAddress::operator=(const IpAddress &other)
	{
		memcpy(mData, other.mData, sizeof(mData));
		mVersion = other.mVersion;

		return *this;
	}

	bool IpAddress::operator==(const IpAddress &other) const
	{
		return ( 0 == memcmp(mData, &other.mData, sizeof(mData)) &&
		         mVersion == other.mVersion);
	}

	bool IpAddress::operator<(const IpAddress &other) const
	{
		return (memcmp(this, &other, sizeof(IpAddress)) < 0);
	}

	StdExt::String IpAddress::toString() const
	{
		if (isValid())
		{
			char strBuffer[64];
			memset(strBuffer, 0, sizeof(strBuffer));

			sockaddr_in* sockIn = (sockaddr_in*)&mData[0];

			if (IpVersion::V4 == mVersion)
			{
				in_addr* addr = (in_addr*)&mData[0];
				inet_ntop(AF_INET, addr, strBuffer, sizeof(strBuffer));
			}
			else if (IpVersion::V6 == mVersion)
			{
				in_addr6* addr = (in_addr6*)&mData[0];
				inet_ntop(AF_INET6, addr, strBuffer, sizeof(strBuffer));
			}

			return StdExt::String(strBuffer);
		}

		return StringLiteral("");
	}

	IpVersion IpAddress::version() const
	{
		return mVersion;
	}

	bool IpAddress::isValid() const
	{
		return (IpVersion::NONE != mVersion);
	}

	bool IpAddress::getSysAddress(in_addr* out) const
	{
		if (nullptr != out && IpVersion::V4 == mVersion)
		{
			const in_addr* addrPtr = reinterpret_cast<const in_addr*>(&mData[0]);
			*out = *addrPtr;

			return true;
		}

		return false;
	}

	bool IpAddress::getSysAddress(in6_addr* out) const
	{
		if (nullptr != out && IpVersion::V6 == mVersion)
		{
			const in6_addr* addrPtr = reinterpret_cast<const in6_addr*>(&mData[0]);
			*out = *addrPtr;

			return true;
		}

		return false;
	}
}

#else
#	error "SysComm::IpAddress not supported."
#endif

namespace StdExt::Serialize
{
	using namespace IpComm;

	constexpr StringLiteral IP_INVALID("IP_INVALID");

	namespace Text
	{
		template<>
		STD_EXT_EXPORT void read(const StdExt::String& string, IpAddress* out)
		{
			*out = IpAddress(string);
		}

		template<>
		STD_EXT_EXPORT StdExt::String write(const IpAddress& val)
		{
			return val.toString();
		}
	}

	namespace Binary
	{
		template<>
		void read<IpAddress>(ByteStream* stream, IpComm::IpAddress *out)
		{
			// OPTIMIZE THIS WITH ACTUAL BINARY IMPLEMENTATION
			*out = Text::read<IpAddress>( read<String>(stream) );
		}

		template<>
		void write<IpAddress>(ByteStream* stream, const IpComm::IpAddress &val)
		{
			// OPTIMIZE THIS WITH ACTUAL BINARY IMPLEMENTATION
			write<String>(stream, Text::write<IpAddress>(val) );
		}
	}
}
