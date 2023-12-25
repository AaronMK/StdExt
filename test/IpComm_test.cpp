#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/IpComm/NetworkInterface.h>
#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/TcpConnection.h>
#include <StdExt/IpComm/Exceptions.h>
#include <StdExt/IpComm/Endpoint.h>
#include <StdExt/IpComm/Udp.h>

#include <StdExt/Concurrent/CallableTask.h>
#include <StdExt/Concurrent/Scheduler.h>

#include <StdExt/Streams/TestByteStream.h>
#include <StdExt/Streams/SocketStream.h>

#include <StdExt/Test/Test.h>

#include <StdExt/Compare.h>
#include <StdExt/Utility.h>

#include <chrono>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::IpComm;
using namespace StdExt::Concurrent;

using namespace std::chrono;

constexpr Port test_port = 12345;

class TestClient : public Task<void>
{
public:
	TestClient(const IpAddress& remote)
		: mRemoteHost(remote)
	{
	}

	bool succeded() const
	{
		return mSucceded;
	}

protected:
	virtual void run() override
	{
		U8String test_message = U8String::literal(u8"Test Message");
		
		TcpConnection connection;
		connection.connect(mRemoteHost, test_port);

		Serialize::Binary::write(&connection, test_message);
		auto received_message = Serialize::Binary::read<U8String>(&connection);

		mSucceded = (test_message == received_message);
	}

private:
	IpAddress mRemoteHost;
	bool mSucceded = false;
};

class TestServer : public Task<void>
{
public:
	TestServer(const IpAddress& local_addr)
		: mBindAddr(local_addr)
	{
	}

protected:
	virtual void run() override
	{
		TcpServer server;
		server.bind(mBindAddr, test_port);

		auto connection = server.getClient();
		auto received_message = Serialize::Binary::read<U8String>(&connection);
		Serialize::Binary::write(&connection, received_message);
	}

private:
	IpAddress mBindAddr;
};

void testIpComm()
{
	auto all_interfaces = NetworkInterface::allInterfaces();

	testForException<format_error>(
		"An invalid address string throws a format_error.",
		[]()
		{
			auto test_addr = IpAddress(u8"Bad String");
		}
	);

#pragma region IPv4 192.168.*.* Local Network Address Test
	{
		IpAddress local_address(u8"192.168.255.201");

		testForResult<IpVersion>(
			"Dotted notation string creates IPv4 address.",
			IpVersion::V4, local_address.version()
		);

		testForResult<bool>(
			"Local IPv4 Address in 192.168.*.* range is considered unique local.",
			true, local_address.isUniqueLocal()
		);

		testForResult<bool>(
			"Local IPv4 Address in 192.168.*.* range is not considered globally unique.",
			false, local_address.isGlobalUnicast()
		);

		IpAddress min_192(u8"192.168.0.0");
		IpAddress max_192(u8"192.168.255.255");

		testForResult<bool>(
			"Local IPv4 Address at each end of the 192.168.*.* range"
			" is considered locally unique.",
			true, min_192.isUniqueLocal() && max_192.isUniqueLocal()
		);

		IpAddress below_min_192(u8"192.167.255.255");
		IpAddress above_max_192(u8"192.169.0.0");

		testForResult<bool>(
			"Local IPv4 Address beyond each end of the 192.168.*.* range"
			" is considered locally unique.",
			false, below_min_192.isUniqueLocal() || above_max_192.isUniqueLocal()
		);

		testForResult<IpAddress>(
			"IPv4 address properly prefixes.",
			IpAddress(u8"192.168.255.0"), local_address.prefix(24)
		);

		testForResult<IpAddress>(
			"IPv4 address properly postfixes.",
			IpAddress(u8"0.0.0.201"), local_address.postfix(8)
		);
	}
#pragma endregion

#pragma region IPv4 172.16-31.*.* Local Network Address Test
	{
		IpAddress local_address(u8"172.20.15.30");

		testForResult<bool>(
			"Local IPv4 Address in 172.16-31.*.* range is considered unique local.",
			true, local_address.isUniqueLocal()
		);

		testForResult<bool>(
			"Local IPv4 Address in 172.16-31.*.* range is not considered globally unique.",
			false, local_address.isGlobalUnicast()
		);

		IpAddress min_172(u8"172.16.0.0");
		IpAddress max_172(u8"172.31.255.255");

		testForResult<bool>(
			"Local IPv4 Address at each end of the 172.16-31.*.* range"
			" is considered locally unique.",
			true, min_172.isUniqueLocal() && max_172.isUniqueLocal()
		);

		IpAddress below_min_172(u8"172.15.255.255");
		IpAddress above_max_172(u8"172.32.0.0");

		testForResult<bool>(
			"Local IPv4 Address beyond each end of the 172.16-31.*.* range"
			" are not considered locally unique.",
			false, below_min_172.isUniqueLocal() || above_max_172.isUniqueLocal()
		);
	}
#pragma endregion

#pragma region IPv4 10.*.*.* Local Network Address Test
	{
		IpAddress local_address(u8"10.18.65.29");

		testForResult<bool>(
			"Local IPv4 Address in 10.*.*.* range is considered unique local.",
			true, local_address.isUniqueLocal()
		);

		testForResult<bool>(
			"Local IPv4 Address in 10.*.*.* range is not considered globally unique.",
			false, local_address.isGlobalUnicast()
		);

		IpAddress min_10(u8"10.0.0.0");
		IpAddress max_10(u8"10.255.255.255");

		testForResult<bool>(
			"Local IPv4 Address at each end of the 10.*.*.* range"
			" is considered locally unique.",
			true, min_10.isUniqueLocal() && max_10.isUniqueLocal()
		);

		IpAddress below_min_10(u8"9.255.255.255");
		IpAddress above_max_10(u8"11.0.0.0");

		testForResult<bool>(
			"Local IPv4 Address beyond each end of the 10.*.*.* range"
			" are not considered locally unique.",
			false, below_min_10.isUniqueLocal() || above_max_10.isUniqueLocal()
		);
	}
#pragma endregion

#pragma region IPv4 224.0.0.0/4 Multicast Address Test
	{
		IpAddress min_multi(u8"224.0.0.0");
		IpAddress max_multi(u8"239.255.255.255");

		IpAddress below_min_multi(u8"223.255.255.255");
		IpAddress above_max_multi(u8"240.0.0.0");

		testForResult<bool>(
			"IPv4 Addresses at each end of the 224.0.0.0/4 Multicast range"
			" is properly considered multicast.",
			true, min_multi.isMulticast() && max_multi.isMulticast()
		);


		testForResult<bool>(
			"IPv4 addresses beyond each end of the 224.0.0.0/4 Multicast range"
			" are not considered multicast.",
			false, below_min_multi.isMulticast() || above_max_multi.isMulticast()
		);
	}
#pragma endregion

#pragma region IPv6 Globally Unique
	{
		IpAddress min_gu(u8"2000::");
		IpAddress max_gu(u8"3fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

		IpAddress below_min_gu(u8"1fff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
		IpAddress above_max_gu(u8"40::");

		testForResult<bool>(
			"Min and max IPv6 global unicast addresses are recognized as such.",
			true, min_gu.isGlobalUnicast() && max_gu.isGlobalUnicast()
		);

		testForResult<bool>(
			"IPv6 addresses beyond each end of the global unicast range"
			" are not considered global unicast.",
			false, below_min_gu.isGlobalUnicast() || above_max_gu.isGlobalUnicast()
		);
	}
#pragma endregion

#pragma region IPv6 Link Local
	{
		IpAddress min_ll(u8"fe80::");
		IpAddress max_ll(u8"febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

		IpAddress below_min_ll(u8"fe7f:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
		IpAddress above_max_ll(u8"fec0::");

		testForResult<bool>(
			"Min and max IPv6 link local addresses are recognized as such.",
			true, min_ll.isLinkLocal() && max_ll.isLinkLocal()
		);

		testForResult<bool>(
			"IPv6 addresses beyond each end of the link local range"
			" are not considered global unicast.",
			false, below_min_ll.isLinkLocal() || above_max_ll.isLinkLocal()
		);
	}
#pragma endregion

#pragma region IPv6 Unique Local
	{
		IpAddress min_ul(u8"fc00::");
		IpAddress max_ul(u8"fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

		IpAddress below_min_ul(u8"fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
		IpAddress above_max_ul(u8"fe00::");

		testForResult<bool>(
			"Min and max IPv6 unique local addresses are recognized as such.",
			true, min_ul.isUniqueLocal() && max_ul.isUniqueLocal()
		);

		testForResult<bool>(
			"IPv6 addresses beyond each end of the unique local range"
			" are not considered global unicast.",
			false, below_min_ul.isUniqueLocal() || above_max_ul.isUniqueLocal()
		);
	}
#pragma endregion

#pragma region IPv6 Multicast
	{
		IpAddress min_mc(u8"ff00::");
		IpAddress max_mc(u8"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

		IpAddress below_min_mc(u8"feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

		testForResult<bool>(
			"Min and max IPv6 multicast addresses are recognized as such.",
			true, min_mc.isMulticast() && max_mc.isMulticast()
		);

		testForResult<bool>(
			"IPv6 address below multicast range is not considered mutlicast",
			false, below_min_mc.isMulticast()
		);
	}
#pragma endregion

	{
		TestServer test_server(IpAddress::loopback(IpVersion::V6));
		TestClient test_client(IpAddress::loopback(IpVersion::V6));

		Scheduler scheduler;
		scheduler.addTask(test_server);
		scheduler.addTask(test_client);

		test_server.wait();
		test_client.wait();

		Test::testForResult<bool>(
			"IPv6 local host server and client connected and exchanged data.",
			true, test_client.succeded()
		);
	}

	{
		TestServer test_server(IpAddress::loopback(IpVersion::V4));
		TestClient test_client(IpAddress::loopback(IpVersion::V4));

		Scheduler scheduler;
		scheduler.addTask(test_server);
		scheduler.addTask(test_client);

		test_server.wait();
		test_client.wait();

		Test::testForResult<bool>(
			"IPv4 local host server and client connected and exchanged data.",
			true, test_client.succeded()
		);
	}

	testForException<IpComm::EndpointInUse>(
		"EndpointInUse exception is thrown if two servers"
		" try to attach to the same endpoint.",
		[] (){
			TcpServer test_server_1;
			TcpServer test_server_2;

			test_server_1.bind(IpAddress::loopback(IpVersion::V4), 12345);
			test_server_2.bind(IpAddress::loopback(IpVersion::V4), 12345);
		}
	);

	testForException<IpComm::ConnectionRejected>(
		"Connection is refused when there is no server running on the port.",
		[] (){
			TcpConnection test_connection;
			test_connection.connect(IpAddress::loopback(IpVersion::V4), 12345);
		}
	);

	testForException<IpComm::TimeOut>(
		"Connection times out when a receive timeout is set, but server does not send data.",
		[] (){
			TcpServer test_server;
			test_server.bind(IpAddress::loopback(IpVersion::V4), 12345);

			TcpConnection test_connection;
			test_connection.connect(IpAddress::loopback(IpVersion::V4), 12345);
			test_connection.setReceiveTimeout(milliseconds(500));

			std::array<char, 10> buffer;
			test_connection.receive(buffer.data(), buffer.size());
		}
	);

	testForException<IpComm::TimeOut>(
		"Receive times out when a timeout is set, but server does not send all expected data.",
		[] (){
			String test_string(u8"Test String");
			String test_substring = test_string.substr(0, test_string.size() / 2 );
			
			Streams::TestByteStream test_stream;
			Serialize::Binary::write(&test_stream, test_string);

			auto expected_write_size = test_stream.getSeekPosition();

			auto server_task = makeTask(
				[&]()
				{
					TcpServer test_server;
					test_server.bind(IpAddress::loopback(IpVersion::V4), 12345);

					auto recv_connection = test_server.getClient();
					Serialize::Binary::write(&recv_connection, test_substring);
				}
			);

			Scheduler scheduler;
			scheduler.addTask(server_task);
			
			try
			{
				auto cleanup_task = finalBlock(
					[&]()
					{
						server_task.wait();
					}
				);

				TcpConnection test_connection;
				test_connection.connect(IpAddress::loopback(IpVersion::V4), 12345);
				test_connection.setReceiveTimeout(seconds(5));

				std::vector<char> buffer(expected_write_size);
				test_connection.readRaw(buffer.data(), buffer.size());
			}
			catch (const std::exception&)
			{
				throw;
			}
		}
	);

	{
		constexpr Port test_port = 12200;
		
		Udp client_udp;
		client_udp.bind(IpVersion::V4);

		Udp server_udp;
		server_udp.bind(IpAddress(u8"127.0.0.1"), test_port);

		const String request_string(u8"Request Message");
		const String response_string(u8"Response Message");

		bool receive_succeeded = false;
		
		auto client_task = makeTask(
			[&]()
			{
				auto destination = server_udp.localEndpoint();
				Streams::SocketStream stream;

				Serialize::Binary::write(&stream, request_string);
				client_udp.sendPacket(stream.dataPtr(0), stream.bytesAvailable(), destination);

				stream.clear();
				stream.write(256, [&](void* data, size_t size)
					{
						return client_udp.receivePacket(data, size);
					}
				);

				receive_succeeded = ( response_string == Serialize::Binary::read<String>(&stream) );
			}
		);

		auto server_task = makeTask(
			[&]()
			{
				Streams::SocketStream stream;
				Endpoint remote_sender;

				stream.write(256, [&](void* data, size_t size)
					{
						return server_udp.receivePacket(data, size, &remote_sender);
					}
				);

				String response = ( request_string == Serialize::Binary::read<String>(&stream) ) ?
					response_string : String(u8"Bad Request");

				stream.clear();
				Serialize::Binary::write(&stream, response);

				server_udp.sendPacket(stream.dataPtr(0), stream.bytesAvailable(), remote_sender);
			}
		);

		Scheduler scheduler;
		scheduler.addTask(server_task);
		scheduler.addTask(client_task);

		server_task.wait();
		client_task.wait();

		testForResult<bool>(
			"Upd successfully receives a packet and responds",
			true, receive_succeeded
		);
	}
}