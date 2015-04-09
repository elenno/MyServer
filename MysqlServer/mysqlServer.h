#ifndef __MYSQL_SERVER_H__
#define __MYSQL_SERVER_H__

#include "tcpServer.h"
#include "tcpConnection.h"

namespace my
{
	class MysqlServer : public TcpServer
	{
	public:
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef boost::shared_ptr<MysqlServer> ptr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
	
	public:
		MysqlServer();
		~MysqlServer();
        
		void init();
		void asyncAccept();
		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // ÷ÿ–¥handle_accept

	private:
		Json::Value m_SvrConf;
		AcceptorPtr m_pAcceptor;
		ConnectionPtr m_GameConn;
	};
}

#endif


