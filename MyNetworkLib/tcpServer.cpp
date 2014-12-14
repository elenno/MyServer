#include "tcpServer.h"
#include "tcpConnection.h"
#include "messageHandler.h"

my::TcpServer::TcpServer()
{
	init();
}

my::TcpServer::~TcpServer()
{

}

void my::TcpServer::init()
{
	m_pService = ServicePtr(new boost::asio::io_service());
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint());
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(*m_pService, *m_pEndpoint));
	BasePtr msgHandler(new MessageHandler());
	ConnectionPtr conn(new TcpConnection(*m_pService, msgHandler));
	m_pAcceptor->async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
}

void my::TcpServer::run()
{
	printf("start io_service thread!\n");
	boost::recursive_mutex::scoped_lock lock(mtx);
	boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&io_service::run, m_pService)));
	thread->detach();
	m_ServiceThreads.push_back(thread);
}

void my::TcpServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	printf("start!\n");
	if (err)
	{
		printf("%s\n",err.message().c_str());
		return;
	}
//	m_Service.post(boost::bind(&TcpConnection::start, conn));
	conn->start();
	std::cout << conn->getSocket().remote_endpoint().address() << " " << conn->getSocket().remote_endpoint().port() << std::endl;
	BasePtr msgHandler(new MessageHandler());
	conn.reset(new TcpConnection(*m_pService, msgHandler));
	m_pAcceptor->async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
}