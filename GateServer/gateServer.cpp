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
	m_nNetIdHolder = 0;
	m_pService = ServicePtr(new boost::asio::io_service());
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(*m_pService, *m_pEndpoint));

	GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));

	connectToGameSvr(gameSvrIp, gameSvrPort);
	connectToAccountSvr(accountSvrIp, accountSvrPort);

	update();
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
		
		conn->setNetId(m_nNetIdHolder);
		m_ConnMap.insert(std::make_pair<int, ConnectionPtr>(m_nNetIdHolder, conn));		
		conn->start();
		m_nNetIdHolder = (m_nNetIdHolder + 1) % MAX_NET_ID;
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
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId(), msg.getNetId());
	tmp.serialize();
	if(!m_pGameConn->sendMessage(tmp))
	{
		LogW << "send msg to gameSvr FAILED!!! len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
	}
}

void my::GateServer::sendToAccountSvr(NetMessage& msg)
{
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId(), msg.getNetId());
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
	int netId = msg.getNetId();
	ConnectionMap::iterator it = m_PlayerMap.find(playerId);
	bool flag = false;
	if (it == m_PlayerMap.end())
	{
		LogW << __FUNCTION__ << "| Player not online, playerId=" << playerId << LogEnd;
		it = m_ConnMap.find(netId);
		flag = true;
		if (it == m_ConnMap.end())
		{
			LogW << __FUNCTION__ << "| ConnectionMap not found netId:" << netId << LogEnd;
		    return;
		}
	}
	ConnectionPtr playerConn = it->second;
	if (!playerConn->getSocket().is_open())
	{
		//socket already closed!!!
		LogW << __FUNCTION__ << "| Socket closed! netId=" << netId << LogEnd;
		//kick player
		kickConnection(playerConn);
		return;
	}

	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId(), msg.getNetId());
	tmp.serialize();
	if (!playerConn->sendMessage(tmp))
	{
		LogW << __FUNCTION__ << "| Send Msg To Player Failed, playerId=" << playerId << LogEnd;
	}
}

void my::GateServer::onPlayerLogin(int playerId, int netId)
{
	if (netId < 0 || playerId < 0)
	{
		return;
	}
	ConnectionMap::iterator it = m_PlayerMap.find(playerId);
	if (it != m_PlayerMap.end())
	{
		ConnectionPtr conn = it->second;
		if (conn->getNetId() == netId)
		{
			//玩家已在线，无需再登陆
			return;
		}
		kickConnection(conn);
	}
    it = m_ConnMap.find(netId);
	if (it == m_ConnMap.end())
	{
		//找不到conn,哪里有问题？
		LogW << __FUNCTION__ << "  Can't find connection, netId=" << netId << LogEnd;
	}
	else
	{
		ConnectionPtr conn = it->second;
		conn->setPlayerId(playerId);
		conn->setHeartBeat(m_SystemTime); //登陆的时候心跳一次
		m_PlayerMap.insert(ConnectionMap::_Val_type(playerId, conn));
		LogD << __FUNCTION__ << "  New User Login, playerId=" << playerId << " netId=" << netId << LogEnd;
	}
}

void my::GateServer::kickPlayer(int playerId, int netId)
{
	if (playerId > 0)
	{
		ConnectionMap::iterator it = m_PlayerMap.find(playerId);
		if (it != m_PlayerMap.end() && netId == (it->second)->getNetId())
		{
			m_PlayerMap.erase(it);
		}
	}
}

void my::GateServer::kickConnection(ConnectionPtr conn)
{
	int netId = conn->getNetId();
	if (netId < 0)
	{
		//server, don't do anything stupid
	}
	else
	{
		boost::recursive_mutex::scoped_lock lock(mtx);
		m_ConnMap.erase(netId);
		conn->stop();
		m_nConnCount--;
		int playerId = conn->getPlayerId();
		kickPlayer(playerId, netId);
	}
}

void my::GateServer::update()
{
	boost::system_time tmp = boost::get_system_time();
	time_t c = (tmp - m_SystemTime).total_milliseconds();
	if (c >= 5000)
	{
		time_t now = time(NULL);

		//检查心跳
		checkHeartBeat(tmp);
	}
	m_SystemTime = tmp;
	HelpFunctions::threadSleep(1);
	m_pService->post(boost::bind(&GateServer::update, this));
}

void my::GateServer::checkHeartBeat(boost::system_time tmp)
{
	ConnectionMap::iterator it = m_ConnMap.begin();
	for (; it != m_ConnMap.end(); )
	{
		ConnectionPtr conn = it->second;
		++it;
		if ((tmp - conn->getHeartBeat()).total_seconds() > 60)//一分钟没心跳，死了吧
		{
			kickConnection(conn); //todo kick
		}
	}
}

boost::system_time my::GateServer::getSystemTime()
{
	return m_SystemTime;
}