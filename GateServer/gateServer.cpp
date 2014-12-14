#include "gateServer.h"
#include "gateHandler.h"
#include "log_system.h"
#include "helpFunctions.h"
#include "fileSystem.h"
#include "stringDef.h"
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
	
	std::string ipStr = "127.0.0.1";
	int	port = gateConf["port"].asInt();
	std::string gameSvrIp = gateConf["gameSvrIp"].asString();
	int gameSvrPort = gateConf["gameSvrPort"].asInt();

	m_nConnCount = 0;
	m_pService = ServicePtr(new boost::asio::io_service());
	m_pEndpoint = EndpointPtr(new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipStr), port));
	m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(*m_pService, *m_pEndpoint));

	connectToGameSvr(gameSvrIp, gameSvrPort);
}

void my::GateServer::connectToGameSvr(std::string ipaddr, int port)
{
	std::string portStr = boost::lexical_cast<std::string, int>(port);
	GateHandler::ptr gatehandler = boost::shared_ptr<GateHandler>(new GateHandler());
	m_pGameConn = boost::shared_ptr<TcpConnection>(new TcpConnection(*m_pService, gatehandler));
	m_pGameConn->setNetId(-1);
	connect(ipaddr, portStr, m_pGameConn);
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
		m_nConnCount++;
		conn->start();
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

