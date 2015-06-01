#include "mysqlServer.h"
#include "mysqlHandler.h"
#include "log_system.h"
#include "valueDef.h"
#include "core.h"
#include "fileSystem.h"
#include "stringDef.h"

my::MysqlServer::MysqlServer()
{

}

my::MysqlServer::~MysqlServer()
{

}

void my::MysqlServer::init()
{
	boost::shared_ptr<TcpServer> serverPtr(this); //make sure that shared_from_this() can run perfectly ok!
	m_SvrConf = util::fileSystem::loadJsonFileEval(jsonconf::server_config);
	if (m_SvrConf == Json::nullValue)
	{
		LogW << "Error init MysqlServer, null mysqlConf" << LogEnd;
		return;
	}
	int	port = m_SvrConf["mysqlSvrPort"].asInt();
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getService(), *m_pEndpoint));
	asyncAccept();

	LogD << "Init Mysql Server OK!!!" << LogEnd;
}

void my::MysqlServer::asyncAccept()
{
	MysqlHandler::ptr mysqlHandler = boost::shared_ptr<MysqlHandler>(new MysqlHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), mysqlHandler, shared_from_this()));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&MysqlServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
}

void my::MysqlServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		LogW << "Accept Error, reason=" << err.message() << LogEnd;
		conn->stop();
		asyncAccept();
		return;
	}

	try {
		if (conn->getSocket().remote_endpoint().address().to_string() != m_SvrConf["gameSvrIp"].asString() ||
			 conn->getSocket().remote_endpoint().port() != m_SvrConf["gameSvrPort"].asUInt())
		{
			LogW << "unknown incoming guest : " << conn->getSocket().remote_endpoint().address().to_string() << LogEnd;
			m_GameConn->stop();
		}
		else
		{
			LogD << "Accept GameSvr!!" << LogEnd;
			m_GameConn = conn;
			m_GameConn->setNetId(server_id::GAME_SVR);
			static ip::tcp::no_delay option(true);
			m_GameConn->getSocket().set_option(option);
			m_GameConn->start();
		}
		asyncAccept();
	}catch(std::exception& e)
	{
		LogE << "Caught Exception:  reason=" << e.what() << LogEnd;
	}
}

void my::MysqlServer::handle_disconnect(ConnectionPtr conn)
{
	LogD << "this is mysql server!" << LogEnd;
}