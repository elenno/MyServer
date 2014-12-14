#include "tcpServer.h"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "gateServer.h"

int main()
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::endpoint en(boost::asio::ip::address::from_string("127.0.0.1"), 10085);
	//TcpServer server(io_service, en);
	//server.run();

	my::GateServer gateSvr;
	gateSvr.init();
	gateSvr.run();
	while(1)
	{

	}
	return 0;
}