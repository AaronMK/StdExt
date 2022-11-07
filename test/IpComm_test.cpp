#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/IpComm/NetworkInterface.h>
#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/TcpConnection.h>
#include <StdExt/IpComm/Exceptions.h>
#include <StdExt/IpComm/Endpoint.h>
#include <StdExt/IpComm/Udp.h>

#include <StdExt/Concurrent/FunctionTask.h>
#include <StdExt/Concurrent/CallableTask.h>

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

class TestClient : public Task
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

class TestServer : public Task
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

	testForException<std::format_error>(
		"An invalid address string throws a std::format_error.",
		[]()
		{
			auto test_addr = IpAddress(u8"Bad String");
		}
	);

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
			" is considered locally unique.",
			false, below_min_172.isUniqueLocal() || above_max_172.isUniqueLocal()
		);
	}

	{
		TestServer test_server(IpAddress::loopback(IpVersion::V6));
		TestClient test_client(IpAddress::loopback(IpVersion::V6));

		test_server.runAsync();
		test_client.runAsync();

		waitForAll({ &test_server, &test_client });

		Test::testForResult<bool>(
			"IPv6 local host server and client connected and exchanged data.",
			true, test_client.succeded()
		);
	}

	{
		TestServer test_server(IpAddress::loopback(IpVersion::V4));
		TestClient test_client(IpAddress::loopback(IpVersion::V4));

		test_server.runAsync();
		test_client.runAsync();

		waitForAll( {&test_server, &test_client} );

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

			Concurrent::FunctionTask server_task(
				[&]()
				{
					TcpServer test_server;
					test_server.bind(IpAddress::loopback(IpVersion::V4), 12345);

					auto recv_connection = test_server.getClient();
					Serialize::Binary::write(&recv_connection, test_substring);
				}
			);

			server_task.runAsync();
			
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

		server_task.runAsync();
		client_task.runAsync();

		waitForAll({&server_task, &client_task});

		testForResult<bool>(
			"Upd successfully receives a packet and responds",
			true, receive_succeeded
		);
	}
}