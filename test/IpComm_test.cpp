#include <StdExt/IpComm/IpAddress.h>
#include <StdExt/IpComm/NetworkInterface.h>
#include <StdExt/IpComm/TcpServer.h>
#include <StdExt/IpComm/TcpConnection.h>

#include <StdExt/Concurrent/FunctionTask.h>

#include <StdExt/Test/Test.h>

using namespace StdExt;
using namespace StdExt::Test;
using namespace StdExt::IpComm;
using namespace StdExt::Concurrent;

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
}