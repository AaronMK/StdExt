#ifndef _STD_EXT_IP_COMM_SOCK_ADDR_H_
#define _STD_EXT_IP_COMM_SOCK_ADDR_H_

#include "IpAddress.h"

#include <array>

namespace StdExt::IpComm
{
	/**
	 * @brief
	 *  Encapsulates a native sockaddr data structure, and wraps functionality around it
	 *  that handles the casting and logic of populating and parsing the structure.
	 */
	class STD_EXT_EXPORT SockAddr
	{

	public:
		SockAddr();

		SockAddr(const in_addr& addr, Port port);
		SockAddr(const in6_addr& addr, Port port);

		SockAddr(const Endpoint& endpoint);
		SockAddr(const IpAddress& addr, Port port);

		/**
		 * @brief
		 *  Gets a sockaddr pointer to the data suitable for low-level socket functions
		 *  that need it as an input parameter.
		 */
		const sockaddr* data() const;

		/**
		 * @brief
		 *  Gets a pointer to the internal buffer used to store the sockaddr structure.
		 *  This is suitable for passing as an output parameter to low-level socket functions
		 *  that will populate a socket address.
		 */
		sockaddr* data();

		/**
		 * @brief
		 *  Gets the size of the stored sockaddr structure.
		 */
		socklen_t size() const;

		/**
		 * @brief
		 *  Gets a pointer to the internal size member for use as an in/out
		 *  for low-level sockets functions that populate the address structure.
		 *  The size field will be set to the internal size of the buffer before
		 *  a pointer to it is returned.
		 */
		socklen_t* sizeInOut();

		IpVersion version() const;

		Endpoint toEndpoint() const;

	private:
		static constexpr socklen_t max_sockaddr_size =
			Number::convert<socklen_t>( std::max( sizeof(sockaddr_in), sizeof(sockaddr_in6) ) );
		
		std::array<std::byte, max_sockaddr_size> mData;
		socklen_t mSize;
	};
}

#endif // !_STD_EXT_IP_COMM_SOCK_ADDR_H_