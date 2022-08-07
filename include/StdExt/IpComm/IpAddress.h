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

		/**
		 * @brief
		 *  Constructs an invalid %IpAddress.
		 */
		IpAddress();

		/**
		 * @brief
		 *  Constructs an IP address from a string, automatically
		 *  determining the version.
		 */
		IpAddress(const char* addr);

		/**
		 * @brief
		 *  Constructs an IP address from a string, automatically
		 *  determining the version.
		 */
		IpAddress(const std::string& addr);

		/**
		* @brief
		*  Constructs an IP address from a string, automatically
		*  determining the version.
		*/
		IpAddress(const StdExt::String& addr);

		/**
		 * @brief
		 *  Constructs an IP address of the passed version from a string.
		 *
		 *  IPv4 addresses must be in dotted notation. (ie. 123.45.6.7)
		 */
		IpAddress(const char* addr, IpVersion version);

		/**
		 * @brief
		 *  Constructs an IP address of the passed version from a string.
		 *
		 *  IPv4 addresses must be in dotted notation. (ie. 123.45.6.7)
		 */
		IpAddress(const std::string& addr, IpVersion version);

		/**
		* @brief
		*  Constructs an IP address of the passed version from a string.
		*
		*  IPv4 addresses must be in dotted notation. (ie. 123.45.6.7)
		*/
		IpAddress(const StdExt::String& addr, IpVersion version);

		IpAddress(const in_addr* addr);
		IpAddress(const in6_addr* addr);

		bool getSysAddress(in_addr* out) const;
		bool getSysAddress(in6_addr* out) const;

		IpAddress& operator=(const IpAddress& other);

		std::strong_ordering operator<=>(const IpAddress& other) const;
		
		StdExt::String toString() const;
		IpVersion version() const;

		bool isValid() const;

	private:
		uint8_t mData[16];
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