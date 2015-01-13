#include "tcpServer.h"
#include "tcpConnection.h"
#include "messageHandler.h"
#include "core.h"
#include "log_system.h"

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
	LogD << "Server started!!!" << LogEnd;
}

void my::TcpServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		LogE << err.message() << LogEnd;
		return;
	}
	conn->start();
	try
	{
		std::cout << conn->getSocket().remote_endpoint().address() << " " << conn->getSocket().remote_endpoint().port() << std::endl;
		BasePtr msgHandler(new MessageHandler());
		conn.reset(new TcpConnection(core.getService(), msgHandler));
		m_pAcceptor->async_accept(conn->getSocket(), boost::bind(&TcpServer::handle_accept, this, conn, boost::asio::placeholders::error));
	}
	catch(std::exception& e)
	{
		LogE << e.what() << LogEnd;
	}
}