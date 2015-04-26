#include "connection.h"
#include "core.h"
#include <nedmalloc/nedmalloc.h>
#include "http_common.hpp"
#include <boost/lexical_cast.hpp>

namespace my
{
	namespace http
	{
		Connection::Connection() : m_Socket(core.getHttpService())
		{

		}

		Connection::~Connection()
		{

		}

		Connection::ptr Connection::create()
		{
			void* m = nedalloc::nedmalloc(sizeof(Connection));
			return ptr(new(m) Connection(),Connection::destroy);
		}

		void Connection::destroy(Connection *p)
		{
			p->~Connection();
			nedalloc::nedfree(p);
		}

		ip::tcp::socket& Connection::getSocket()
		{
			return m_Socket;
		}

		void Connection::start()
		{
			readMsg();
		}

		void Connection::stop()
		{
			m_Socket.close();
		}

		void Connection::readMsg()
		{
			try
			{
				if (!m_Socket.is_open())
				{
					//LogD << "socket is closed" << LogEnd;
					return;
				}
				m_Socket.async_read_some(boost::asio::buffer(m_ReadBuffer), boost::bind(&Connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}catch (std::exception& e)
			{
				//LogE << "Caught Exception: netId=" << m_nNetId << "  reason=" << e.what() << LogEnd;
			}
		}

		void Connection::writeMsg(NetMessage& msg)
		{
			reply rep;
			rep.status = reply::ok;

			std::string stmp = msg.getMessage();
			//std::cout << stmp << std::endl;
			//rep.content = "{\"code\":0,\"content\":" + stmp + "}";
			rep.content = stmp;
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string, int>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = "application/json";

			m_Reply = rep;
			boost::asio::async_write(m_Socket, m_Reply.to_buffers(),
				boost::bind(&Connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error));
		}

		void Connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
		{
			//parse  and  make it a Json::Value then transport it to gate, let gate deal with it
		}

		void Connection::handle_write(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec); //why shutdown
			}

			if (e != boost::asio::error::operation_aborted)
			{
				//http_conn_mgr->stop(this->connection_id);
			}
		}
	}
}
