#ifndef __HTTP_SERVER_H__ 
#define __HTTP_SERVER_H__

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.h"
#include "tcpServer.h"

using namespace boost::asio::ip;
namespace my
{
	namespace http
	{
		class HttpServer : private boost::noncopyable, public my::TcpServer
		{
		public:
			typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

		public:
			HttpServer();
			virtual ~HttpServer();

			void run();
			void init(const Json::Value& gateConfig);
			void stop();

		private:
			void startAccept();
			void handleAccept(Connection::ptr cPtr, const boost::system::error_code& e);

		private:
			AcceptorPtr m_pAcceptor;
		};
	}
}
#endif