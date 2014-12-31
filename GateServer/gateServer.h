#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/thread/thread_time.hpp>
#include <string>
#include "tcpServer.h"
#include "tcpConnection.h"

using namespace boost::asio;
#define gateSvr boost::detail::thread::singleton<my::GateServer>::instance()

#define MAX_NET_ID 2147483647

namespace my
{
	class GateServer : public TcpServer
	{
	public:
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef std::map<int, ConnectionPtr> ConnectionMap;
		typedef boost::shared_ptr<GateServer> ptr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

	public:
		GateServer();
		~GateServer();

		void connectToGameSvr(std::string ipaddr, int port);
		void connectToAccountSvr(std::string ipaddr, int port);

		void sendToGameSvr(NetMessage& msg);
		void sendToAccountSvr(NetMessage& msg);
		void sendToPlayer(NetMessage& msg);

		void init();

		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // 重写handle_accept
		void handle_connect(ConnectionPtr conn, boost::system::error_code err);

		void onPlayerLogin(int playerId, int netId);	
		void update();
		boost::system_time getSystemTime();

	private:
		void connect(std::string ipaddr, std::string port, ConnectionPtr conn);
		void checkHeartBeat(boost::system_time time);
		void kickConnection(ConnectionPtr conn);
		void kickPlayer(int playerId, int netId);

	private:

		AcceptorPtr m_pAcceptor;
		ConnectionMap m_ConnMap;
		ConnectionMap m_PlayerMap;
		int m_nConnCount;
		int m_nNetIdHolder;
		ConnectionPtr m_pGameConn; // 用于连接到game
		ConnectionPtr m_pAccountConn; //连接AcountSvr
		boost::system_time m_SystemTime;
	};
}

#endif