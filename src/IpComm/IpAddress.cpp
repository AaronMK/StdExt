#include <StdExt/IpComm/IpAddress.h>

#include <StdExt/Serialize/Exceptions.h>

namespace StdExt::IpComm
{
	static std::array<uint8_t, 16> toArray(const in_addr& addrV4)
	{
		std::array<uint8_t, 16> ret;
		uint32_t hs_addr = ntohl(addrV4.s_addr);

		ret[0] = (hs_addr & 0xff000000) >> 24;
		ret[1] = (hs_addr & 0x00ff0000) >> 16;
		ret[2] = (hs_addr & 0x0000ff00) >> 8;
		ret[3] = (hs_addr & 0x000000ff);

		for (size_t i = 4; i < ret.size(); ++i)
			ret[i] = 0;

		return ret;
	}

	static std::array<uint8_t, 16> toArray(const in6_addr& addrV6)
	{
		std::array<uint8_t, 16> ret;
		ret.fill(0);

		for (size_t i = 0; i < ret.size(); ++i)
			ret[i] = addrV6.s6_addr[i];

		return ret;
	}

	static std::array<uint8_t, 16> toArray(std::span<const uint16_t, 8> addrV6)
	{
		std::array<uint8_t, 16> ret;
		ret.fill(0);

		for (size_t i = 0; i < addrV6.size(); ++i)
		{
			ret[i] = (addrV6[i] & 0xff00) >> 8;
			ret[i + 1] = addrV6[i] & 0x00ff;
		}

		return ret;
	}

	static in_addr toV4addr(std::span<const uint8_t, 4> addr)
	{
		uint32_t out_uint = 0;
		out_uint += addr[0] << 24;
		out_uint += addr[1] << 16;
		out_uint += addr[2] << 8;
		out_uint += addr[3];

		in_addr ret;
		memset(&ret, 0, sizeof(in_addr));

		ret.s_addr = htonl(out_uint);

		return ret;
	}

	static in_addr toV4addr(std::span<const uint8_t, 16> addr)
	{
		return toV4addr( addr.subspan<0, 4>() );
	}

	static in_addr6 toV6addr(std::span<const uint8_t, 16> addr)
	{
		in_addr6 ret;
		memset(&ret, 0, sizeof(in_addr6));

		for (size_t i = 0; i < addr.size(); ++i)
			ret.s6_addr[i] = addr[i];

		return ret;
	}
	
	IpAddress IpAddress::any(IpVersion version)
	{
		if (IpVersion::V4 == version)
		{
			in_addr addr;
			memset(&addr, 0, sizeof(in_addr));

			#ifdef _WIN32
				addr.S_un.S_addr = INADDR_ANY;
			#else
				addr.s_addr = INADDR_ANY;
			#endif

			return IpAddress(addr);
		}
		else if (IpVersion::V6 == version)
		{
			return IpAddress(in6addr_any);
		}

		return IpAddress();
	}

	IpAddress IpAddress::loopback(IpVersion version)
	{
		constexpr std::array<uint8_t, 4> IPv4Loopback{127, 0, 0, 1};
		constexpr std::array<uint8_t, 16> IPv6Loopback {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
		};

		switch (version)
		{
		case IpVersion::V6:
			return IpAddress(IPv6Loopback);
		case IpVersion::V4:
			return IpAddress(IPv4Loopback);
		default:
			return IpAddress();
		}
	}

	IpAddress::IpAddress()
	{
		mData.fill(0);
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

	IpAddress::IpAddress(std::span<const uint8_t, 4> parts)
		: IpAddress()
	{
		mVersion = IpVersion::V4;
		std::copy_n(parts.begin(), 4, mData.begin());
	}

	IpAddress::IpAddress(const std::array<uint8_t, 4>& parts)
		: IpAddress()
	{
		mVersion = IpVersion::V4;
		std::copy_n(parts.begin(), 4, mData.begin());
	}

	IpAddress::IpAddress(std::span<const uint8_t, 16> parts)
	{
		mVersion = IpVersion::V6;
		std::copy_n(parts.begin(), 16, mData.begin());
	}

	IpAddress::IpAddress(const std::array<uint8_t, 16>& parts)
	{
		mVersion = IpVersion::V6;
		mData = parts;
	}

	IpAddress::IpAddress(std::span<const uint16_t, 8> parts)
	{
		mVersion = IpVersion::V6;
		mData = toArray(parts);
	}

	IpAddress::IpAddress(const std::array<uint16_t, 8>& parts)
	{
		mVersion = IpVersion::V6;
		mData = toArray(parts);
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
		: IpAddress()
	{
		if (IpVersion::V4 == version)
		{
			in_addr addrV4;
			if ( 1 == inet_pton(AF_INET, addr, &addrV4) )
			{
				mVersion = IpVersion::V4;
				mData = toArray(addrV4);
			}
		}
		else if (IpVersion::V6 == version)
		{
			in6_addr addrV6;
			if ( 1 == inet_pton(AF_INET6, addr, &addrV6) )
			{
				mVersion = IpVersion::V6;
				mData = toArray(addrV6);
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

	IpAddress::IpAddress(const in_addr& addr)
	{
		mVersion = IpVersion::V4;
		mData = toArray(addr);
	}

	IpAddress::IpAddress(const in6_addr& addr)
	{
		mVersion = IpVersion::V6;
		mData = toArray(addr);
	}

	std::strong_ordering IpAddress::operator<=>(const IpAddress& other) const
	{
		if ( mVersion == IpVersion::NONE && other.mVersion == IpVersion::NONE )
			return std::strong_ordering::equivalent;

		if ( mVersion != other.mVersion )
			return mVersion <=> other.mVersion;

		size_t comp_max = ( IpVersion::V4 == mVersion ) ? 4 : 16;

		for (size_t i = 0; i < comp_max; ++i)
		{
			auto result = mData[i] <=> other.mData[i];

			if ( 0 != result )
				return result;
		}

		return std::strong_ordering::equivalent;
	}

	StdExt::String IpAddress::toString() const
	{
		if (isValid())
		{
			char strBuffer[64];
			memset(strBuffer, 0, sizeof(strBuffer));

			if (IpVersion::V4 == mVersion)
			{
				in_addr addr = toV4addr(mData);
				inet_ntop(AF_INET, &addr, strBuffer, sizeof(strBuffer));
			}
			else if (IpVersion::V6 == mVersion)
			{
				in6_addr addr = toV6addr(mData);
				inet_ntop(AF_INET6, &addr, strBuffer, sizeof(strBuffer));
			}

			return StdExt::String(strBuffer);
		}

		return String::literal("");
	}

	IpVersion IpAddress::version() const
	{
		return mVersion;
	}

	bool IpAddress::isValid() const
	{
		return (IpVersion::NONE != mVersion);
	}


	in_addr IpAddress::getSysIPv4() const
	{
		if (IpVersion::V4 != mVersion)
			throw invalid_operation("Cannot convert to system IPv4 address.");

		return toV4addr(mData);
	}

	in6_addr IpAddress::getSysIPv6() const
	{
		if (IpVersion::V6 != mVersion)
			throw invalid_operation("Cannot convert to system IPv6 address.");

		return toV6addr(mData);
	}
}

namespace StdExt::Serialize
{
	using namespace IpComm;

	String IP_INVALID = String::literal("IP_INVALID");

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
