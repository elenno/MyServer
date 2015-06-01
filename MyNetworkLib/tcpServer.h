#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "tcpConnection.h"
#include "baseHandler.h"

using namespace boost::asio;

namespace my
{
	class BaseHandler;
	class TcpConnection;
	class TcpServer : public boost::enable_shared_from_this<TcpServer>
	{
	public:
		typedef boost::shared_ptr<BaseHandler> BasePtr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::endpoint> EndpointPtr;

		TcpServer();
		virtual ~TcpServer();

		void run();
		void init();
		void handle_accept(ConnectionPtr conn, boost::system::error_code err);
		virtual void handle_disconnect(ConnectionPtr conn);

	protected:
		boost::recursive_mutex mtx;
		EndpointPtr m_pEndpoint;
		AcceptorPtr m_pAcceptor;
	};
}

#endif