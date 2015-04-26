#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include "message.h"
#include "http_common.hpp"

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

		public:
			int m_nConnId;

		private:
			void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
			void handle_write(const boost::system::error_code& e);

		private:
			ip::tcp::socket m_Socket;
			
			//char m_ReadBuffer[8192];
			char m_ReadBuffer[8192];
			char m_WriteBuffer[8192];
			int m_nWriteLen;   
			bool m_bWriteInProgress;
			bool m_bReadInProgress;
			reply m_Reply;
		};
	}
}



#endif