#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "tcpConnection.h"
#include "baseHandler.h"

using namespace boost::asio;

namespace my
{
	class BaseHandler;
	class TcpConnection;
	class TcpServer
	{
	public:
		typedef boost::shared_ptr<BaseHandler> BasePtr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::endpoint> EndpointPtr;

		TcpServer();
		~TcpServer();

		void run();
		void init();
		void handle_accept(ConnectionPtr conn, boost::system::error_code err);

	protected:
		boost::recursive_mutex mtx;
		EndpointPtr m_pEndpoint;
		AcceptorPtr m_pAcceptor;
	};
}

#endif