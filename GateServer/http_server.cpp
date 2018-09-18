#include "http_server.h"
#include "core.h"
#include <boost/lexical_cast.hpp>
#include "connection.h"
#include "connection_manager.h"

namespace my
{
	namespace http
	{
		HttpServer::HttpServer()
		{

		}

		HttpServer::~HttpServer()
		{

		}

		void HttpServer::init()
		{
			//read port and doc_root from json config
			//init
			std::string port = "10088";
			std::string doc_root = "F://";
			int iport = boost::lexical_cast<int, std::string>(port);
			m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getService(), tcp::endpoint(tcp::v4(), iport)));
			m_pAcceptor->set_option(tcp::acceptor::reuse_address(true)); //why set to reuse??
			m_pAcceptor->listen(); //why listen?
			startAccept();
		}

		void HttpServer::run()
		{
			core.run();
		}

		void HttpServer::stop()
		{
			m_pAcceptor->close();
		}

		void HttpServer::startAccept()
		{

			Connection::ptr cPtr = Connection::create();
			m_pAcceptor->async_accept(cPtr->getSocket(), boost::bind(&HttpServer::handleAccept, this, cPtr, boost::asio::placeholders::error));

		}

		void HttpServer::handleAccept(Connection::ptr cPtr, const boost::system::error_code& e)
		{
			if (!e)
			{
				httpConnMgr.start(cPtr);
			}
			startAccept();
		}
	}
}

