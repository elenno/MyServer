#include "connection.h"
#include "core.h"
#include <nedmalloc/nedmalloc.h>
#include "http_common.hpp"
#include <boost/lexical_cast.hpp>
#include "log_system.h"
#include <boost/logic/tribool.hpp>
#include<boost/tuple/tuple.hpp>
#include "gateServer.h"
#include <boost/filesystem.hpp>
#include "fileSystem.h"

namespace my
{
	namespace http
	{
		Connection::Connection() : m_Socket(core.getService())
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
					//kick connection
					return;
				}
				m_Socket.async_read_some(boost::asio::buffer(m_ReadBuffer), boost::bind(&Connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}catch (std::exception& e)
			{
				//kick connection
				LogE << "Caught Exception: " << "  reason=" << e.what() << LogEnd;
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

		void Connection::writeResourceMsg(std::string& content)
		{
			reply rep;
			rep.status = reply::ok;
			//std::cout << stmp << std::endl;
			//rep.content = "{\"code\":0,\"content\":" + stmp + "}";
			rep.content = content;
			rep.headers.resize(3);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string, int>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = "text/html";
			rep.headers[2].name = "Server";
			rep.headers[2].value = "http_server powered by Lynn";

			m_Reply = rep;
			boost::asio::async_write(m_Socket, m_Reply.to_buffers(),
				boost::bind(&Connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error));
		}

		void Connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
		{
			//parse  and  make it a Json::Value then transport it to gate, let gate deal with it
			m_bReadInProgress = false;
			if (!e)
			{
				//mutex lock
				boost::recursive_mutex::scoped_lock(m_ReadMtx);
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = m_RequestParser.parse(
					m_Request, m_ReadBuffer.data(), m_ReadBuffer.data() + bytes_transferred);

				if (result)
				{
					//request_handler_.handle_request(request_, reply_);
					//boost::asio::async_write(m_Socket, m_Reply.to_buffers(),
					//	boost::bind(&Connection::handle_write, shared_from_this(),
					//	boost::asio::placeholders::error));
					
					LogW << m_Request.uri << LogEnd;

					if (!is_service_command(m_Request.uri))
					{
						if (!is_request_resource(m_Request.uri))
						{
							m_Reply = reply::stock_reply(reply::bad_request);
							boost::asio::async_write(m_Socket, m_Reply.to_buffers(),
								boost::bind(&Connection::handle_write, shared_from_this(),
								boost::asio::placeholders::error));
							return;
						}
						else
						{
							//get a file
							boost::filesystem::path rss_path(m_Request.uri);
							boost::filesystem::path cur_cpath = boost::filesystem::current_path();
							boost::filesystem::path file_path = cur_cpath / rss_path; //path支持重载/运算符
							if (!boost::filesystem::exists(file_path) || !boost::filesystem::is_regular_file(file_path))
							{
								m_Reply = reply::stock_reply(reply::bad_request);
								boost::asio::async_write(m_Socket, m_Reply.to_buffers(),
									boost::bind(&Connection::handle_write, shared_from_this(),
									boost::asio::placeholders::error));
								return;
							}
							std::string content = util::fileSystem::loadJsonFile(file_path.string());
							writeResourceMsg(content);
							return;
						}
					}

					Json::Value jreq;
					int ret = 0;
					if (m_Request.method == "GET")
					{
						ret = m_RequestParser.parse_content(GET_METHOD, m_Request.uri, jreq);
					}
					else
					{
						ret = m_RequestParser.parse_content(POST_METHOD,  m_Request.content, jreq);
					}
					if (ret != 0)
					{
						m_Reply = reply::stock_reply(reply::bad_request);
						boost::asio::async_write(m_Socket, m_Reply.to_buffers(),
							boost::bind(&Connection::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
						return;
					}
					jreq["net_id"] = this->m_nConnId;
					LogW << jreq.toStyledString() << LogEnd;
					
					gateSvr.on_http_req(this, jreq);
					NetMessage netMsg;
					netMsg.setMessage("{\"status\":\"ok\"}");
					writeMsg(netMsg);
				}
				else if (!result)
				{
					m_Reply = reply::stock_reply(reply::bad_request);
					boost::asio::async_write(m_Socket, m_Reply.to_buffers(),					
						boost::bind(&Connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error));
				}
				else
				{
					m_Socket.async_read_some(boost::asio::buffer(m_ReadBuffer),	
						boost::bind(&Connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				}
			}
			else
			{
				//失败返回bad request
			}
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

		bool Connection::is_service_command(string uri)
		{
			return uri.find("/service?") == 0;
		}

		bool Connection::is_request_resource(string uri)
		{
			return uri.find("/www/") == 0;
		}

	}
}
