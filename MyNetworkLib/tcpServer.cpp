#include "tcpServer.h"
#include "tcpConnection.h"
#include "messageHandler.h"
#include "core.h"

my::TcpServer::TcpServer()
{
	init();
}

my::TcpServer::~TcpServer()
{

}

void my::TcpServer::init()
{
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint());
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getService(), *m_pEndpoint));
	BasePtr msgHandler(new MessageHandler());
	ConnectionPtr conn(new TcpConnection(core.getService(), msgHandler));
	m_pAcceptor->async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
}

void my::TcpServer::run()
{
	core.run();
	std::cout << "Server started!!!" << std::endl;
}

void my::TcpServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		printf("%s\n",err.message().c_str());
		return;
	}
	conn->start();
	std::cout << conn->getSocket().remote_endpoint().address() << " " << conn->getSocket().remote_endpoint().port() << std::endl;
	BasePtr msgHandler(new MessageHandler());
	conn.reset(new TcpConnection(core.getService(), msgHandler));
	m_pAcceptor->async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
}