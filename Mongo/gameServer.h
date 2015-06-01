#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "tcpServer.h"
#include "tcpConnection.h"
#include "message.h"
#include "common.h"
#include <queue>
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
		typedef std::queue<NetMessage> MessageQueue;

	public:
		GameServer();
		virtual ~GameServer();

		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // ÷ÿ–¥handle_accept
		virtual void handle_disconnect(ConnectionPtr conn);
		bool init();
		void asyncAccept();
		void update();
		void pushMessage(NetMessage& msg);
		void runMessage();

	private:
		ConnectionPtr m_GateConn;
		AcceptorPtr m_pAcceptor;
		boost::system_time m_SystemTime;
		MessageQueue m_MsgQueue;
	};
}

#endif