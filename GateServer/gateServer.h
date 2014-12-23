#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <string>
#include "tcpServer.h"
#include "tcpConnection.h"

using namespace boost::asio;
#define gateSvr boost::detail::thread::singleton<my::GateServer>::instance()

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

		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // ��дhandle_accept
		void handle_connect(ConnectionPtr conn, boost::system::error_code err);

		void onPlayerLogin(int playerId);

	private:
		void connect(std::string ipaddr, std::string port, ConnectionPtr conn);

	private:

		AcceptorPtr m_pAcceptor;
		ConnectionMap m_ConnMap;
		ConnectionMap m_PlayerMap;
		int m_nConnCount;
		ConnectionPtr m_pGameConn; // �������ӵ�game
		ConnectionPtr m_pAccountConn; //����AcountSvr

	};
}

#endif