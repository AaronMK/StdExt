#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/Serialize/Exceptions.h>

#include <StdExt/Memory/BitMask.h>
#include <StdExt/Memory/Endianess.h>

#include <StdExt/Compare.h>

#include <format>
#include <limits>

#ifdef STD_EXT_APPLE
	// with Apple clang, std::format_error is not in <format>,
	// but is somewhere in the include chain for <chrono>
#	include <chrono>
#endif

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

	static in6_addr toV6addr(std::span<const uint8_t, 16> addr)
	{
		in6_addr ret;
		memset(&ret, 0, sizeof(in6_addr));

		for (size_t i = 0; i < addr.size(); ++i)
			ret.s6_addr[i] = addr[i];

		return ret;
	}

	IpAddress::IpAddress(const IpAddress&) = default;
	IpAddress::IpAddress(IpAddress&&) = default;

	IpAddress& IpAddress::operator=(const IpAddress&) = default;
	IpAddress& IpAddress::operator=(IpAddress&&) = default;
	
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

		throw std::format_error("Invalid IpAddress string.");
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

	IpAddress::IpAddress(const StdExt::String& addr)
		: IpAddress(convertString<char>(addr).data())
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

	IpAddress::IpAddress(const StdExt::String& addr, IpVersion version)
		: IpAddress(convertString<char>(addr).data(), version)
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

		auto comp_result = compare(
			mVersion, other.mVersion,
			from_big_endian( access_as<const uint64_t&>(&mData[0]) ),
			from_big_endian( access_as<const uint64_t&>(&other.mData[0]) ),
			from_big_endian( access_as<const uint64_t&>(&mData[8]) ),
			from_big_endian( access_as<const uint64_t&>(&other.mData[8]) )
		);

		return ( comp_result < 0 ) ? std::strong_ordering::less :
			( comp_result > 0 ) ? std::strong_ordering::greater :
			std::strong_ordering::equivalent;
	}

	bool IpAddress::operator==(const IpAddress& other) const
	{
		return 0 == (*this <=> other);
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

			return convertString<char8_t>( CString::literal(strBuffer) );
		}

		return String::literal(u8"");
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

	bool IpAddress::isGlobalUnicast() const
	{
		if ( IpVersion::V4 == mVersion )
		{
			return 
				(10 != mData[0]) &&
				(172 != mData[0] || std::clamp<uint8_t>(mData[1], 16, 31) != mData[1]) &&
				(192 != mData[0] || 168 != mData[1]);
		}
		else if ( IpVersion::V6 == mVersion )
		{
			return (maskBits<7, 5>(mData[0]) == 0x20);
		}

		return false;
	}

	bool IpAddress::isLinkLocal() const
	{
		if ( IpVersion::V6 == mVersion )
		{
			return 
				0xFE == mData[0] &&
				maskBits<7, 6>(mData[1]) == 0x80;
		}

		return false;
	}

	bool IpAddress::isLoopback() const
	{
		if ( IpVersion::NONE == mVersion )
			return false;

		return 0 == (*this <=> IpAddress::loopback(mVersion));
	}

	bool IpAddress::isUniqueLocal() const
	{
		if ( IpVersion::V4 == mVersion )
		{
			return !isGlobalUnicast();
		}
		else if ( IpVersion::V6 == mVersion )
		{
			return 0xFC == (prefixMask<uint8_t>(7) & mData[0]);
		}

		return false;
	}

	bool IpAddress::isMulticast() const
	{
		if ( IpVersion::V4 == mVersion )
		{
			return (mData[0] & prefixMask<uint8_t>(4)) == 224;
		}
		else if ( IpVersion::V6 == mVersion )
		{
			return 0xFF == mData[0];
		}

		return false;
	}

	bool IpAddress::isSolicitedMulticast() const
	{
		uint64_t high_bits = from_big_endian( access_as<const uint64_t&>(&mData[0]) );
		uint64_t low_bits  = from_big_endian( access_as<const uint64_t&>(&mData[8]) );

		constexpr uint64_t low_mask = prefixMask<uint64_t>(40);

		return 
			0xFF02000000000000 == high_bits &&
			( low_mask & low_bits) == 0x1FF000000;
	}

	IpAddress IpAddress::getSolicitedMulticast() const
	{
		if ( IpVersion::V6 != mVersion )
			throw invalid_operation("Can't get a solicited multicast from a non-IPv6 address.");
		
		IpAddress result;

		access_as<uint64_t&>(&result.mData[0]) = 0xFF02000000000000;
			
		uint64_t low_bits  = from_big_endian( access_as<const uint64_t&>(&mData[8]) );
		access_as<uint64_t&>(&result.mData[8]) = 
			to_big_endian(
				0x1FF000000 + ( postfixMask<uint64_t>(6) & low_bits )
		);

		return result;
	}

	IpAddress IpAddress::prefix(uint8_t bit_count) const
	{
		IpAddress result = *this;

		if ( IpVersion::V6 == mVersion )
		{
			if ( bit_count <= 64 )
			{
				access_as<uint64_t&>(&result.mData[0]) &= 
					to_big_endian( prefixMask<uint64_t>(bit_count) );
				access_as<uint64_t&>(&result.mData[8]) = 0;

			}
			else
			{
				access_as<uint64_t&>(&result.mData[8]) &= 
					to_big_endian( prefixMask<uint64_t>(bit_count - 64) );
			}
		}
		else if ( IpVersion::V4 == mVersion )
		{
			access_as<uint32_t&>(&result.mData[0]) &= 
				to_big_endian( prefixMask<uint32_t>(bit_count) );
		}

		return result;
	}

	IpAddress IpAddress::postfix(uint8_t bit_count) const
	{
		IpAddress result = *this;

		if ( IpVersion::V6 == mVersion )
		{
			if ( bit_count <= 64 )
			{
				access_as<uint64_t&>(&result.mData[8]) &= 
					to_big_endian( postfixMask<uint64_t>(bit_count) );
				access_as<uint64_t&>(&result.mData[0]) = 0;
			}
			else
			{
				access_as<uint64_t&>(&result.mData[8]) &= 
					to_big_endian( postfixMask<uint64_t>(bit_count - 64) );
			}
		}
		else if ( IpVersion::V4 == mVersion )
		{
			access_as<uint32_t&>(&result.mData[0]) &= 
				to_big_endian( postfixMask<uint32_t>(bit_count) );
		}

		return result;
	}
}

namespace StdExt::Serialize
{
	using namespace IpComm;

	String IP_INVALID = String::literal(u8"IP_INVALID");

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
