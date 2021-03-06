#ifndef __ACCOUNT_SERVER_H__
#define __ACCOUNT_SERVER_H__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <string>
#include "tcpServer.h"
#include "tcpConnection.h"
#include "accountHandler.h"
#include <json/json.h>

using namespace boost::asio;
#define accountSvr boost::detail::thread::singleton<my::AccountServer>::instance()

namespace my
{
	class AccountServer : public TcpServer
	{
	public:
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef boost::shared_ptr<AccountServer> ptr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

	public:
		AccountServer();
		virtual ~AccountServer();

		void init();
		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // ��дhandle_accept
		virtual void handle_disconnect(ConnectionPtr conn);
		void asyncAccept();

	private:
        
		Json::Value m_SvrConf;
		AcceptorPtr m_pAcceptor;
		ConnectionPtr m_GateConn;
	};
}

#endif