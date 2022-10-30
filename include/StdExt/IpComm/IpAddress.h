#ifndef _STD_EXT_IP_COMM_IP_ADDRESS_H_
#define _STD_EXT_IP_COMM_IP_ADDRESS_H_

#include "../StdExt.h"
#include "IpComm.h"

#include <StdExt/Serialize/Binary/Binary.h>
#include <StdExt/Serialize/Text/Text.h>

#include <StdExt/String.h>

struct in_addr;
struct in6_addr;

namespace StdExt::IpComm
{
	enum class IpVersion
	{
		NONE,
		V4,
		V6
	};

	/**
	 * @brief
	 *  Stores an IPv4 or IPv6 address.
	 */
	class STD_EXT_EXPORT IpAddress final
	{
	public:
		/**
		 * @brief
		 *  Returns an IP address that denotes "any" interface for
		 *  the passed IP version.
		 */
		static IpAddress any(IpVersion version);
		static IpAddress loopback(IpVersion version);

		IpAddress(const IpAddress&) = default;
		IpAddress(IpAddress&&) = default;

		IpAddress& operator=(const IpAddress&) = default;
		IpAddress& operator=(IpAddress&&) = default;

		/**
		 * @brief
		 *  Constructs an invalid %IpAddress.
		 */
		IpAddress();

		/**
		 * @brief
		 *  Creates an IPv4 address from the passed octets, with parts[0]
		 *  being the left-most octet in dotted notation.
		 */
		IpAddress(std::span<const uint8_t, 4> parts);

		/**
		 * @brief
		 *  Creates an IPv4 address from the passed octets, with parts[0]
		 *  being the left-most octet in dotted notation.
		 */
		IpAddress(const std::array<uint8_t, 4>& parts);

		/**
		 * @brief
		 *  Creates an IPv6 address from the passed octets, with parts[0]
		 *  being the left-most octet in dotted notation.
		 */
		IpAddress(std::span<const uint8_t, 16> parts);

		/**
		 * @brief
		 *  Creates an IPv6 address from the passed octets, with parts[0]
		 *  being the left-most octet in dotted notation.
		 */
		IpAddress(const std::array<uint8_t, 16>& parts);

		/**
		 * @brief
		 *  Creates an IPv6 address from the passed 16-bit unsigned ints,
		 *   with parts[0] being the left-most part in the IPv6 notation.
		 */
		IpAddress(std::span<const uint16_t, 8> parts);

		/**
		 * @brief
		 *  Creates an IPv6 address from the passed 16-bit unsigned ints,
		 *   with parts[0] being the left-most part in the IPv6 notation.
		 */
		IpAddress(const std::array<uint16_t, 8>& parts);

		/**
		* @brief
		*  Constructs an IP address from a string, automatically
		*  determining the version.
		*/
		IpAddress(const StdExt::String& addr);

		/**
		* @brief
		*  Constructs an IP address of the passed version from a string.
		*/
		IpAddress(const StdExt::String& addr, IpVersion version);

		/**
		 * @brief
		 *  Constructs a IPv4 address using an in_addr structure.
		 */
		IpAddress(const in_addr& addr);

		/**
		 * @brief
		 *  Constructs a IPv6 address using an in6_addr structure.
		 */
		IpAddress(const in6_addr& addr);
		
		in_addr getSysIPv4() const;
		in6_addr getSysIPv6() const;

		std::strong_ordering operator<=>(const IpAddress& other) const;
		
		StdExt::String toString() const;
		IpVersion version() const;

		bool isValid() const;

	private:

		/**
		 * @brief
		 *  Constructs an IP address from a string, automatically
		 *  determining the version.
		 */
		IpAddress(const char* addr);
		
		/**
		 * @brief
		 *  Constructs an IP address of the passed version from a string.
		 */
		IpAddress(const char* addr, IpVersion version);

		std::array<uint8_t, 16> mData;
		IpVersion mVersion;
	};

	struct Endpoint
	{
		IpAddress address;
		Port      port{};

		Endpoint() = default;

		Endpoint(const sockaddr* addr, socklen_t len)
		{
			static_assert( sizeof(sockaddr_in) != sizeof(sockaddr_in6) );

			if ( len == sizeof(sockaddr_in) )
			{
				sockaddr_in* sockAddr4 = access_as<sockaddr_in*>(addr);

				address = IpAddress(sockAddr4->sin_addr);
				port = ntohs(sockAddr4->sin_port);
			}
			else if ( len == sizeof(sockaddr_in6) )
			{
				sockaddr_in6* sockAddr6 = access_as<sockaddr_in6*>(addr);

				address = IpAddress(sockAddr6->sin6_addr);
				port = ntohs(sockAddr6->sin6_port);
			}
		}
		
		Endpoint(const IpAddress& _addr, Port _port)
			: address(_addr), port(_port)
		{
		}

		/**
		 * @brief
		 *  Creates an end point that denotes a specific port
		 *  but on any interface.
		 */
		Endpoint(IpVersion _version, Port _port)
			: address(IpAddress::any(_version)), port(_port)
		{
			
		}
	};
}

namespace StdExt::Serialize
{
	namespace Text
	{
		template<>
		STD_EXT_EXPORT void read(const StdExt::String& string, IpComm::IpAddress* out);

		template<>
		STD_EXT_EXPORT StdExt::String write(const IpComm::IpAddress& val);
	}

	namespace Binary
	{
		template<>
		STD_EXT_EXPORT void read<IpComm::IpAddress>(ByteStream* stream, IpComm::IpAddress *out);

		template<>
		STD_EXT_EXPORT void write<IpComm::IpAddress>(ByteStream* stream, const IpComm::IpAddress &val);
	}
}

#endif // _STD_EXT_IP_COMM_IP_ADDRESS_H_