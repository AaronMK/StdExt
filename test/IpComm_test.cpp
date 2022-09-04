#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/IpComm/NetworkInterface.h>
#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/TcpConnection.h>
#include <StdExt/IpComm/Exceptions.h>

#include <StdExt/Concurrent/FunctionTask.h>

#include <StdExt/Streams/TestByteStream.h>

#include <StdExt/Test/Test.h>

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
}