#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "tcpServer.h"
#include "tcpConnection.h"
#include <boost/thread/detail/singleton.hpp>

#define gameSvr boost::detail::thread::singleton<my::GameServer>::instance()

namespace my
{
	class GameServer : public TcpServer
	{
	public:
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef std::map<int, ConnectionPtr> ConnectionMap;
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

	public:
		GameServer();
		~GameServer();

		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // ÷ÿ–¥handle_accept
		void init();
		void asyncAccept();

	private:
		ConnectionPtr m_GateConn;
		AcceptorPtr m_pAcceptor;
	};
}

#endif