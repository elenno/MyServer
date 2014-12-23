#include "accountServer.h"
#include "fileSystem.h"
#include "log_system.h"
#include "stringDef.h"

my::AccountServer::AccountServer()
{

}

my::AccountServer::~AccountServer()
{

}

void my::AccountServer::init()
{
	m_AccountConf = my::fileSystem::loadJsonFileEval(jsonconf::accountConf);
	if (m_AccountConf == Json::nullValue)
	{
		LogW << "Error init GateServer, null gateConf" << LogEnd;
		return;
	}
	int	port = m_AccountConf["port"].asInt();
	m_pService = ServicePtr(new boost::asio::io_service());
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(*m_pService, *m_pEndpoint));
	asyncAccept();
}

void my::AccountServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		std::cout << err.message() << std::endl;
		conn->getSocket().close();
		asyncAccept();
	}
	else if (conn->getSocket().remote_endpoint().address().to_string() != m_AccountConf["gateSvrIp"].asString()
		&& conn->getSocket().remote_endpoint().port() != m_AccountConf["gateSvrPort"].asUInt())//判断连接是否gate，不是则断开连接
	{
		std::cout << "unknown incoming guest : " << conn->getSocket().remote_endpoint().address().to_string() << std::endl;
		conn->getSocket().close();
		asyncAccept();
	}
	else
	{
		m_GateConn = conn;
		m_GateConn->start();
		asyncAccept();
	}
}

void my::AccountServer::asyncAccept()
{
	AccountHandler::ptr accountHandler = boost::shared_ptr<AccountHandler>(new AccountHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, accountHandler));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&AccountServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
}