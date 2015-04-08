#include "gateServer.h"
#include "gateHandler.h"
#include "log_system.h"
#include "helpFunctions.h"
#include "fileSystem.h"
#include "stringDef.h"
#include "valueDef.h"
#include <boost/lexical_cast.hpp>
#include <json/json.h>
#include "core.h"

my::GateServer::GateServer()
{

}

my::GateServer::~GateServer()
{

}

void my::GateServer::init()
{
	Json::Value gateConf = util::fileSystem::loadJsonFileEval(jsonconf::server_config);
	if (gateConf == Json::nullValue)
	{
		LogW << "Error init GateServer, null gateConf" << LogEnd;
		return;
	}
	m_GateConf = gateConf;
	int	port = gateConf["gateSvrPort"].asInt();
	std::string gameSvrIp = gateConf["gameSvrIp"].asString();
	int gameSvrPort = gateConf["gameSvrPort"].asInt();
	std::string accountSvrIp = gateConf["accountSvrIp"].asString();
	int accountSvrPort = gateConf["accountSvrPort"].asInt();

	m_nConnCount = 0;
	m_nNetIdHolder = 0;
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getService(), *m_pEndpoint));

	m_GateHandler = boost::shared_ptr<GateHandler>(new GateHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), m_GateHandler));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));

	connectToGameSvr(gameSvrIp, gameSvrPort);
	connectToAccountSvr(accountSvrIp, accountSvrPort);

	LogD << "Init Gate Server Ok!!!" << LogEnd;

	update();
}

void my::GateServer::connectToGameSvr(std::string ipaddr, int port)
{
	std::string portStr = boost::lexical_cast<std::string, int>(port);
	m_pGameConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), m_GateHandler));
	m_pGameConn->setNetId(my::server_id::GAME_SVR);
	connect(ipaddr, portStr, m_pGameConn);
}

void my::GateServer::connectToAccountSvr(std::string ipaddr, int port)
{
	std::string portStr = boost::lexical_cast<std::string, int>(port);
	m_pAccountConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), m_GateHandler));
	m_pAccountConn->setNetId(my::server_id::ACCOUNT_SVR);
	connect(ipaddr, portStr, m_pAccountConn);
}

void my::GateServer::handle_connect(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		LogW << "server name: " << conn->getNetId() << "  connect error: " << err.message() << LogEnd;
		if (conn->getSocket().is_open())
		{
			LogW << "Close Socket!!" << LogEnd;
			conn->getSocket().close();
		}
		return;
	}
	else
	{
		//输出一下这个是成功连接了哪个服务器
		LogD << "server name: " << conn->getNetId() << "  connect success!" << LogEnd;
		try
		{
			static ip::tcp::no_delay option(true);
			conn->getSocket().set_option(option);
			//start
			conn->start();
		}catch(std::exception& e)
		{
			LogE << "Connect to server EXCEPTION!!! server=" << conn->getNetId() << "  reason=" << e.what() << LogEnd;
		}	
	}
}

void my::GateServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		LogE << err.message() << LogEnd;
		conn->getSocket().close();
		ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), m_GateHandler));
		m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
	}
	else
	{
		//new incomming player!!!
		LogD << conn->getSocket().remote_endpoint().address() << " " << conn->getSocket().remote_endpoint().port() << LogEnd;

		boost::recursive_mutex::scoped_lock lock(mtx);
		conn->setNetId(m_nNetIdHolder);
		ip::tcp::no_delay option(true);
		conn->getSocket().set_option(option);
		m_ConnMap.insert(std::make_pair<int, ConnectionPtr>(m_nNetIdHolder, conn));		
		conn->start();
		m_nNetIdHolder = (m_nNetIdHolder + 1) % MAX_NET_ID;
		m_nConnCount++;
			
		ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), m_GateHandler));
		m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GateServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
	}
}

void my::GateServer::connect(std::string ipaddr, std::string port, ConnectionPtr conn)
{
	boost::system::error_code err;
	ip::tcp::resolver rslv(core.getService());
	ip::tcp::resolver::query q(ipaddr, port);
	ip::tcp::resolver::iterator iter = rslv.resolve(q, err);
	if (err)
	{
		LogE << err.message() << LogEnd;
		return;
	}
	async_connect(conn->getSocket(), iter, boost::bind(&GateServer::handle_connect, this, conn, boost::asio::placeholders::error));
}

void my::GateServer::sendToGameSvr(NetMessage& msg)
{
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId(), msg.getNetId());
	tmp.serialize();
	if(0 != m_pGameConn->sendMessage(tmp))
	{
		LogW << "send msg to gameSvr FAILED!!! len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
	}
}

void my::GateServer::sendToAccountSvr(NetMessage& msg)
{
	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId(), msg.getNetId());
	tmp.serialize();
	LogD << "send msg to accountSvr, len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
	if(0 != m_pAccountConn->sendMessage(tmp))
	{
		LogW << "send msg to accountSvr FAILED!!! len=" << tmp.getLen() << " proto=" << tmp.getProto() << " msg=" << tmp.getMessage() << LogEnd;
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
		LogW << "| Player not online, try ConnMap, playerId=" << playerId << LogEnd;
		it = m_ConnMap.find(netId);
		flag = true;
		if (it == m_ConnMap.end())
		{
			LogW << "| ConnectionMap not found netId:" << netId << LogEnd;
		    return;
		}
	}
	ConnectionPtr playerConn = it->second;
	if (!playerConn->getSocket().is_open())
	{
		//socket already closed!!!
		LogW << "Socket closed! netId=" << netId << LogEnd;
		//kick player
		kickConnection(playerConn);
		return;
	}

	NetMessage tmp(msg.getMessage(), msg.getProto(), msg.getPlayerId(), msg.getNetId());
	tmp.serialize();
	if (0 != playerConn->sendMessage(tmp))
	{
		LogW << "Send Msg To Player Failed, playerId=" << playerId << LogEnd;
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
		LogW << "  Can't find connection, netId=" << netId << LogEnd;
	}
	else
	{
		ConnectionPtr conn = it->second;
		conn->setPlayerId(playerId);
		conn->setHeartBeat(m_SystemTime); //登陆的时候心跳一次
		m_PlayerMap.insert(ConnectionMap::_Val_type(playerId, conn));
		LogD << "  New User Login, playerId=" << playerId << " netId=" << netId << LogEnd;
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

bool my::GateServer::kickConnection(ConnectionPtr conn)
{
	int netId = conn->getNetId();
	if (netId < 0)
	{
		//server, don't do anything stupid
	}
	else
	{
		boost::recursive_mutex::scoped_lock lock(mtx);
		ConnectionMap::iterator it = m_ConnMap.find(netId);
		if (it != m_ConnMap.end())
		{
			ConnectionPtr tmpConn = it->second;
			m_ConnMap.erase(it);//应检查conn和tmpConn是否相同
			m_nConnCount--;
			int playerId = conn->getPlayerId();
			kickPlayer(playerId, netId);
			tmpConn->stop();
			LogD << "erase from connMap" << LogEnd;
			return true;
		}
	}
	return false;
}

void my::GateServer::update()
{
	boost::system_time tmp = boost::get_system_time();
	time_t c = (tmp - m_SystemTime).total_milliseconds();
	if (c >= 5000)
	{
		m_SystemTime = tmp;
		time_t now = time(NULL);
		
		//检查服务器是否依旧联通
		checkServerAlive(tmp);

		//检查心跳
		checkHeartBeat(tmp);
	}
	util::HelpFunctions::threadSleep(1);
	core.getService().post(boost::bind(&GateServer::update, this));
}

void my::GateServer::checkHeartBeat(boost::system_time tmp)
{
	ConnectionMap::iterator it = m_ConnMap.begin();
	for (; it != m_ConnMap.end(); )
	{
		ConnectionPtr conn = it->second;
		++it;
		if ((tmp - conn->getHeartBeat()).total_seconds() > 180)//一分钟没心跳，死了吧
		{
			LogW << "Connection is dead, kick it! netId=" << conn->getNetId() << "  playerId=" << conn->getPlayerId() << LogEnd;
			kickConnection(conn); //todo kick
		}
	}
}

void my::GateServer::checkServerAlive(boost::system_time time)
{
	if (!m_pAccountConn->getSocket().is_open())
	{
		LogW << "lost connection with accountSvr, reconnecting..." << LogEnd;
		std::string accountSvrIp = m_GateConf["accountSvrIp"].asString();
		int accountSvrPort = m_GateConf["accountSvrPort"].asInt();
		connectToAccountSvr(accountSvrIp, accountSvrPort);
	}
	if (!m_pGameConn->getSocket().is_open())
	{
		LogW << "lost connection with gameSvr, reconnecting..." << LogEnd;
		std::string gameSvrIp = m_GateConf["gameSvrIp"].asString();
		int gameSvrPort = m_GateConf["gameSvrPort"].asInt();
		connectToGameSvr(gameSvrIp, gameSvrPort);
	}
}

boost::system_time my::GateServer::getSystemTime()
{
	return m_SystemTime;
}