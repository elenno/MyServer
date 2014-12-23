#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "tcpClient.h"

using namespace boost::asio;

int main()
{

	std::vector<boost::shared_ptr<boost::thread>> m_ServiceThreads;
	
	io_service* vIos[1];
	ip::tcp::endpoint end(ip::address::from_string("127.0.0.1"), 10085);
	std::vector<my::TcpClient::ptr> vClient;
	std::string prefix = "my_";
	for (int i = 0; i < 1; i++)
	{
		if (i % 1 == 0)
		{
			vIos[i / 1] = new io_service();
		}
		//vEndPoint[i] = new ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 10086);
		my::TcpClient::ptr cli(new my::TcpClient(*vIos[i / 1], end, prefix, i));
		vClient.push_back(cli);
		vClient[i]->post_connect();
		//boost::recursive_mutex::scoped_lock lock(mtx);
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&my::TcpClient::run, vClient[i])));
		thread->detach();
		m_ServiceThreads.push_back(thread);
	//	cli->post_connect();
	//	cli->run();
	}


/*
	io_service ios;
	ip::tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 10086);
	TcpClient::ptr cli(new TcpClient(ios, endpoint));
	cli->post_connect();
	//cli->start();
	//ios.run();
	cli->run();
	cli->stop();
	printf("finish!\n");
	cli->post_connect();
	cli->run();
	printf("finish2222!\n");*/
	while(1)
	{

	}
}