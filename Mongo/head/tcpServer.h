#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "../head/tcpConnection.h"
using namespace boost::asio;

class TcpServer
{
public:
	TcpServer(io_service& ios, ip::tcp::endpoint& en);
	~TcpServer();

	void run();

	void handle_accept(TcpConnection::ptr conn, boost::system::error_code err);

private:
	std::vector<boost::shared_ptr<boost::thread>> m_ServiceThreads;
	io_service& m_Service;
	ip::tcp::acceptor m_Acceptor;
	boost::recursive_mutex mtx;
};


#endif