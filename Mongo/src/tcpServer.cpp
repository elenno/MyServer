#include "../head/tcpServer.h"
#include "../head/tcpConnection.h"

TcpServer::TcpServer(io_service& ios, ip::tcp::endpoint& en) : m_Service(ios), m_Acceptor(ios, en)
{
	TcpConnection::ptr conn(new TcpConnection(ios));
	m_Acceptor.async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
}

TcpServer::~TcpServer()
{

}

void TcpServer::run()
{
	printf("start io_service thread!\n");
	boost::recursive_mutex::scoped_lock lock(mtx);
	boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&io_service::run, &m_Service)));
	thread->detach();
	m_ServiceThreads.push_back(thread);
}

void TcpServer::handle_accept(TcpConnection::ptr conn, boost::system::error_code err)
{
	printf("start!\n");
	if (err)
	{
		printf("%s\n",err.message().c_str());
		return;
	}
//	m_Service.post(boost::bind(&TcpConnection::start, conn));
	conn->start();
	std::cout << conn->getSocket().remote_endpoint().address() << std::endl;
	conn.reset(new TcpConnection(m_Service));
	m_Acceptor.async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
}