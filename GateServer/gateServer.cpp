#include "gateServer.h"
#include "gateHandler.h"
#include "log_system.h"
#include "helpFunctions.h"
#include "fileSystem.h"
#include "stringDef.h"
#include "valueDef.h"
#include <boost/lexical_cast.hpp>
#include <json/json.h>

my::GateServer::GateServer()
{

}

my::GateServer::~GateServer()
{

}

void my::GateServer::init()
{
	Json::Value gateConf = my::fileSystem::loadJsonFileEval(jsonconf::gateConf);
	if (gateConf == Json::nullValue)
	{
		LogW << "Error init GateServer, null gateConf" << LogEnd;
		return;
	}
	
	int	port = gateConf["port"].asInt();
	std::string gameSvrIp = gateConf["gameSvrIp"].asString();
	int gameSvrPort = gateConf["gameSvrPort"].asInt();
	std::string accountSvrIp = gateConf["accountSvrIp"].asString();
	int accountSvrPort = gateConf["accountSvrPort"].asInt();

	m_nConnCount = 0;
	m_pService = ServicePtr(new boost::asio::io_service());
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(*m_pService, *m_pEndpoint));

	GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));

	connectToGameSvr(gameSvrIp, gameSvrPort);
	connectToAccountSvr(accountSvrIp, accountSvrPort);
}

void my::GateServer::connectToGameSvr(std::string ipaddr, int port)
{
	std::string portStr = boost::lexical_cast<std::string, int>(port);
	GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
	m_pGameConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
	m_pGameConn->setNetId(my::server_id::GAME_SVR);
	connect(ipaddr, portStr, m_pGameConn);
}

void my::GateServer::connectToAccountSvr(std::string ipaddr, int port)
{
	std::string portStr = boost::lexical_cast<std::string, int>(port);
	GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
	m_pAccountConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
	m_pAccountConn->setNetId(my::server_id::ACCOUNT_SVR);
	connect(ipaddr, portStr, m_pAccountConn);
}

void my::GateServer::handle_connect(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		std::cout << __FUNCTION__ << err.message() << std::endl;
		conn->getSocket().close();
		return;
	}
	else
	{
		//输出一下这个是成功连接了哪个服务器
		std::cout << "server name: " << conn->getNetId() << std::endl;

		//start
		conn->start();
	}
}

void my::GateServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		std::cout << err.message() << std::endl;
		conn->getSocket().close();
		GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
		ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
		m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
	}
	else
	{
		//new incomming player!!!
		std::cout << conn->getSocket().remote_endpoint().address() << " " << conn->getSocket().remote_endpoint().port() << std::endl;

		boost::recursive_mutex::scoped_lock lock(mtx);
	    m_ConnMap.insert(std::make_pair<int, ConnectionPtr>(m_nConnCount, conn));	
		conn->setNetId(m_nConnCount);
		conn->start();
		m_nConnCount++;
		GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
		ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
		m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
	}
}

void my::GateServer::connect(std::string ipaddr, std::string port, ConnectionPtr conn)
{
	boost::system::error_code err;
	ip::tcp::resolver rslv(*m_pService);
	ip::tcp::resolver::query q(ipaddr, port);
	ip::tcp::resolver::iterator iter = rslv.resolve(q, err);
	if (err)
	{
		std::cout << err.message() << std::endl;
		return;
	}
	async_connect(conn->getSocket(), iter, boost::bind(&GateServer::handle_connect, this, conn, boost::asio::placeholders::error));
}

void my::GateServer::sendToGameSvr(NetMessage& msg)
{
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId());
	tmp.serialize();
	if(!m_pGameConn->sendMessage(tmp))
	{
		LogW << "send msg to gameSvr FAILED!!! len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
	}
}

void my::GateServer::sendToAccountSvr(NetMessage& msg)
{
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId());
	tmp.serialize();
	LogD << "send msg to gameSvr, len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
	if(!m_pAccountConn->sendMessage(tmp))
	{
		LogW << "send msg to gameSvr FAILED!!! len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
	}
}

void my::GateServer::sendToPlayer(NetMessage& msg)
{
	int playerId = msg.getPlayerId();
	ConnectionMap::iterator it = m_PlayerMap.find(playerId);
	if (it == m_PlayerMap.end())
	{
		LogW << __FUNCTION__ << "| Player not online, playerId=" << playerId << LogEnd;
		return;
	}
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId());
	tmp.serialize();
	ConnectionPtr playerConn = it->second;
	if (!playerConn->sendMessage(tmp))
	{
		LogW << __FUNCTION__ << "| Send Msg To Player Failed, playerId=" << playerId << LogEnd;
	}
}