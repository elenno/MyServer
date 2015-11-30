#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include "message.h"
#include "http_common.hpp"
#include "request_parser.hpp"


using namespace boost::asio;
namespace my
{
	namespace http
	{
		class Connection : public boost::enable_shared_from_this<Connection>, private boost::noncopyable
		{
		public:
			typedef boost::shared_ptr<Connection> ptr;

		public:
			explicit Connection();
			~Connection();

		public:
			void writeMsg(NetMessage& msg);
			void readMsg();
			ip::tcp::socket& getSocket();
			void start();
			void stop();
			static void destroy(Connection *p);
			static ptr create();
			inline void set_connection_id(int conn_id){ m_nConnId = conn_id; }
			inline int get_connection_id(){ return m_nConnId; }

		private:
			void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
			void handle_write(const boost::system::error_code& e);
			bool is_uri_header_correct(string uri);

		private:
			ip::tcp::socket m_Socket;
			//char m_ReadBuffer[8192];
			int m_nConnId;
			boost::array<char, 8192> m_ReadBuffer;
			boost::array<char, 8192> m_WriteBuffer;
			int m_nWriteLen;   
			bool m_bWriteInProgress;
			bool m_bReadInProgress;
			reply m_Reply;
			request_parser m_RequestParser;
			request m_Request;
		};
	}
}



#endif