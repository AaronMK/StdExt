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

		/**
		 * @brief
		 *  Constructs an invalid %IpAddress.
		 */
		IpAddress();

		IpAddress(const IpAddress&);
		IpAddress(IpAddress&&);

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

		IpAddress& operator=(const IpAddress&);
		IpAddress& operator=(IpAddress&&);
		
		in_addr getSysIPv4() const;
		in6_addr getSysIPv6() const;

		std::strong_ordering operator<=>(const IpAddress& other) const;
		bool operator==(const IpAddress& other) const;
		
		StdExt::String toString() const;
		IpVersion version() const;

		bool isValid() const;

		/**
		 * @brief
		 *  Returns true if the address is Internet routable.
		 * 
		 * @details
		 *  This is determined by checking that the address is not within the
		 *  ranges of addresses designated for local networks.
		 */
		bool isGlobalUnicast() const;

		/**
		 * @brief
		 *  True if the address is limited to the local link (or subnet) of the network.
		 *  Only applicable to IPv6.
		 * 
		 * @details
		 *  This is applicable to IPv6 since it has designated address ranges
		 *  that can limit them to a local link/subnet.
		 * 
		 *  For IPv4, it can only be determined if an address is Internet routable.
		 *  Determining if it is within a link must be done in conjuction with a
		 *  subnet mask.  Even then, the scope of the address is not necessarily
		 *  limited to the link/subnet.
		 * 
		 *  For IPv4 use isGlobalUnicast() and/or isUniqueLocal() instead.
		 */
		bool isLinkLocal() const;

		/**
		 * @brief
		 *  Returns true if the IP address is the loopback address.
		 */
		bool isLoopback() const;

		/**
		 * @brief
		 *  Returns true if the address is scoped to a private network,
		 *  which could include other network subnets.
		 * 
		 * @details
		 *  For IPv4 this can only conclude that the address is not Internet
		 *  routable.
		 * 
		 *  For IPv6, this means that the address has the scope of an
		 *  entire private network, as opposed to a link local address
		 *  whick will be scoped to only its link/subnet.
		 */
		bool isUniqueLocal() const;

		/**
		 * @brief
		 *  Returns true if the address is a standard multicast.
		 */
		bool isMulticast() const;

		/**
		 * @brief
		 *  Returns true if the address is for solicited multicast.  These
		 *  addresses are typically not used at the application level.
		 */
		bool isSolicitedMulticast() const;

		/**
		 * @brief
		 *  Returns the octets of the IP address.  If an V4 address is stored,
		 *  a span of size 4 will be returned.  If a V6 address is stored, a
		 *  span of size 16 will be returned.  Otherwise, an empty spane will
		 *  be returned.
		 */
		std::span<const uint8_t> getOctets() const;

		/**
		 * @brief
		 *  Returns the octets of the IP address.  If an V4 address is stored,
		 *  a span of size 4 will be returned.  If a V6 address is stored, a
		 *  span of size 16 will be returned.  Otherwise, an empty spane will
		 *  be returned.
		 */
		std::span<uint8_t> getOctets();

		/**
		 * @brief
		 *  Creates an IP address with using the left most bit_count bits
		 *  of this address then right pads the rest with zeroes.
		 */
		IpAddress prefix(uint8_t bit_count) const;
		

		/**
		 * @brief
		 *  Creates an IP address with using the right most bit_count bits
		 *  of this address then left pads the rest with zeroes.
		 */
		IpAddress postfix(uint8_t bit_count) const;

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

		/**
		 * @brief
		 *  Gets the solicited multicast address corresponding to
		 *  the IpAddress.  This is only applicable to V6 addresses,
		 *  and if not a V6 address, an invalid address will be returned.
		 */
		IpAddress getSolicitedMulticast() const;

		alignas(uint64_t) std::array<uint8_t, 16> mData;
		IpVersion mVersion;
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