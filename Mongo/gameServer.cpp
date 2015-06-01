#include "gameServer.h"
#include "gameHandler.h"
#include "fileSystem.h"
#include "log_system.h"
#include "stringDef.h"
#include "valueDef.h"
#include "playerMgr.h"
#include "helpFunctions.h"
#include "funcHandler.h"
#include "core.h"

my::GameServer::GameServer()
{

}

my::GameServer::~GameServer()
{

}

void my::GameServer::handle_accept(ConnectionPtr conn, boost::system::error_code err)
{
	if (err)
	{
		LogW << "accept error: " << err.message() << LogEnd;
		conn->getSocket().close();
		asyncAccept();
	}
	
	else if (conn->getSocket().remote_endpoint().address().to_string() != "127.0.0.1")//判断连接是否gate，不是则断开连接
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
		ip::tcp::no_delay option(true);
		m_GateConn->getSocket().set_option(option);
		m_GateConn->start();
		asyncAccept();
	}
}

bool my::GameServer::init()
{
	boost::shared_ptr<TcpServer> serverPtr(this); //make sure that shared_from_this() can run perfectly ok!
	Json::Value gameConf = util::fileSystem::loadJsonFileEval(jsonconf::server_config);
	if (gameConf == Json::nullValue)
	{
		LogE << "Error init GameServer, null gameConf" << LogEnd;
		return false;
	}

	int	port = gameConf["gameSvrPort"].asInt();
	std::string gameSvrIp = gameConf["gameSvrIp"].asString();
	int gameSvrPort = gameConf["gameSvrPort"].asInt();

	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(ip::tcp::v4(), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getService(), *m_pEndpoint));

	do 
	{
		//所有系统的初始化
		playerMgr;
	} while (0);

	asyncAccept();

	LogD << "Init Game Server OK!!!" << LogEnd;

	update();

	return true;
}

void my::GameServer::asyncAccept()
{
	GameHandler::ptr gameHandler = boost::shared_ptr<GameHandler>(new GameHandler());
	ConnectionPtr nextConn = boost::shared_ptr<TcpConnection>(new TcpConnection(core.getService(), gameHandler, shared_from_this()));
	m_pAcceptor->async_accept(nextConn->getSocket(), boost::bind(&GameServer::handle_accept, this, nextConn, boost::asio::placeholders::error));
}

void my::GameServer::update()
{
	boost::system_time tmp = boost::get_system_time();
	time_t c = (tmp - m_SystemTime).total_milliseconds();
	if (c >= 5000)
	{
		time_t now = time(NULL);
		//每一段时间更新一次

	}
	m_SystemTime = tmp;
	runMessage();
	util::HelpFunctions::threadSleep(1);
	core.getService().post(boost::bind(&GameServer::update, this));
}

void my::GameServer::pushMessage(NetMessage& msg)
{
	boost::recursive_mutex::scoped_lock lock(mtx);
	m_MsgQueue.push(msg);
}

void my::GameServer::runMessage()
{
	boost::recursive_mutex::scoped_lock lock(mtx);
    if (!m_MsgQueue.empty())
	{
		NetMessage& req = m_MsgQueue.front();
		NetMessage rsp;
		funcHandlerMgr.runFuncHandler(req, rsp);
		if (rsp.serialize())
		{
			//可根据msg的protoId，找到对应的返回的conn，暂用gateConn
			m_GateConn->sendMessage(rsp);
		}
		m_MsgQueue.pop();
	}
}

void my::GameServer::handle_disconnect(ConnectionPtr conn)
{
	//save player's data, disconnect him
	LogD << "this is game server!" << LogEnd;
}