#ifndef __TCP_CLIENT_H__ 
#define __TCP_CLIENT_H__

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include "robot.h"

using namespace boost::asio;

namespace my
{
	class TcpClient : public boost::enable_shared_from_this<TcpClient>
	{
	public:

		typedef boost::shared_ptr<TcpClient> ptr;
		TcpClient(io_service& ios, ip::tcp::endpoint& en, std::string prefix, int id);
		~TcpClient();

		void start();
		void run();
		void handle_connect(const boost::system::error_code& err);
		void handle_write(const boost::system::error_code& err, size_t bytes_transferred);
		void handle_read(const boost::system::error_code& err, size_t bytes_transferred);
		void post_connect();
		void post_write();
		void post_read();
		void stop();

	private:
		io_service& m_Service;
		ip::tcp::socket m_Socket;
		ip::tcp::endpoint m_EndPoint;
		boost::asio::streambuf m_SBuf;
		char m_ReadBuff[1024];
		char m_WriteBuff[1024];
		char m_Buff[1024];
		int m_nWriteLen;
		int count;
		Robot m_Robot;
	};
}

#endif