#include <StdExt/IpComm/IpAddress.h>

#include <StdExt/Test/Test.h>

using namespace StdExt;
using namespace StdExt::Test;

void testIpComm()
{
	IpComm::IpAddress default_address;
	auto str = default_address.toString();

	IpComm::IpAddress local_host("127.0.0.1");
	str = local_host.toString();
}