#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "tcpClient.h"
#include "core.h"

using namespace boost::asio;

int main()
{

	std::vector<boost::shared_ptr<boost::thread> > m_ServiceThreads;
	
	ip::tcp::endpoint end(ip::address::from_string("127.0.0.1"), 10085);
	std::vector<my::TcpClient::ptr> vClient;
	std::string prefix = "lin05_";
	for (int i = 0; i < 100; i++)
	{
		//vEndPoint[i] = new ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 10086);
		boost::asio::io_service& s = core.getService();
		//boost::asio::ip::tcp::resolver resol(s);
		//boost::asio::ip::tcp::resolver::query que("127.0.0.1", "http");
		//boost::asio::ip::tcp::resolver::iterator end = resol.resolve(que);
		my::TcpClient::ptr cli(new my::TcpClient(s, end, prefix, i));
		vClient.push_back(cli);
		vClient[i]->post_connect();
		//boost::recursive_mutex::scoped_lock lock(mtx);
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&my::TcpClient::run, vClient[i])));
		thread->detach();
		m_ServiceThreads.push_back(thread);
	//	cli->post_connect();
	//	cli->run();
	}

	while(1)
	{

	}
}