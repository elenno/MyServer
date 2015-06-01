#include "accountServer.h"
#include "fileSystem.h"
#include "log_system.h"
#include "stringDef.h"
#include "valueDef.h"
#include "core.h"

my::AccountServer::AccountServer()
{

}

my::AccountServer::~AccountServer()
{

}

void my::AccountServer::init()
{
	boost::shared_ptr<TcpServer> serverPtr(this); //make sure that shared_from_this() can run perfectly ok!
	m_SvrConf = util::fileSystem::loadJsonFileEval(jsonconf::server_config);
	if (m_SvrConf == Json::nullValue)
	{
		LogW << "Error init AccountServer, null gateConf" << LogEnd;
		return;
	}
	int	port = m_SvrConf["accountSvrPort"].asInt();
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getService(), *m_pEndpoint));
	asyncAccept();

	LogD << "Init Account Server OK!!!" << LogEnd;
}

void my::AccountServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		LogE << err.message() << LogEnd;
		conn->getSocket().close();
		asyncAccept();
	}
	else if (conn->getSocket().remote_endpoint().address().to_string() != m_SvrConf["gateSvrIp"].asString()
		&& conn->getSocket().remote_endpoint().port() != m_SvrConf["gateSvrPort"].asUInt())//判断连接是否gate，不是则断开连接
	{
		LogW << "unknown incoming guest : " << conn->getSocket().remote_endpoint().address().to_string() << LogEnd;
		conn->getSocket().close();
		asyncAccept();
	}
	else
	{
		LogD << "Accepted Gate Connection!!" << LogEnd;
		m_GateConn = conn;
		m_GateConn->setNetId(server_id::GATE_SVR);
		static ip::tcp::no_delay option(true);
		m_GateConn->getSocket().set_option(option);
		m_GateConn->start();
		asyncAccept();
	}
}

void my::AccountServer::asyncAccept()
{
	AccountHandler::ptr accountHandler = boost::shared_ptr<AccountHandler>(new AccountHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), accountHandler, shared_from_this()));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&AccountServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
}

void my::AccountServer::handle_disconnect(ConnectionPtr conn)
{
	LogD << "this is account server!" << LogEnd;
}