#include "http_server.h"
#include "core.h"
#include <boost/lexical_cast.hpp>
#include "connection.h"
#include "connection_manager.h"
#include "log_system.h"

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

		void HttpServer::init(const Json::Value& gateConfig)
		{
			//read port and doc_root from json config
			//init
			try
			{
				std::string port = gateConfig["httpSvrPort"].asString();
				std::string doc_root = gateConfig["httpDocRoot"].asString();
				int iport = boost::lexical_cast<int, std::string>(port);
				m_pAcceptor = AcceptorPtr(new boost::asio::ip::tcp::acceptor(core.getHttpService(), tcp::endpoint(tcp::v4(), iport)));
				m_pAcceptor->set_option(tcp::acceptor::reuse_address(true)); //why set to reuse??
				m_pAcceptor->listen(); //why listen?
				startAccept();
			}
			catch (std::exception& e)
			{
				LogE << e.what() << LogEnd;
			}			
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

