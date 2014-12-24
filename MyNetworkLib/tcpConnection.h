#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include "message.h"
#include "tcpServer.h"
#include "baseHandler.h"

using namespace boost::asio;

namespace my
{
	class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
	{
	public:
		typedef boost::shared_ptr<BaseHandler> BasePtr;
		TcpConnection(io_service& ios, BasePtr handler);
		~TcpConnection();

		void start();

		ip::tcp::socket& getSocket();
		void setNetId(int netId);
		int getNetId();
		bool sendMessage(NetMessage& msg);
        void stop();

	private:

		void handle_write(const boost::system::error_code& err, size_t bytes_transferred);
		void handle_read(const boost::system::error_code& err, size_t bytes_transferred);
		void post_write();
		void post_read();	

		ip::tcp::socket m_Socket;
		char m_ReadBuffer[8192];
		char m_WriteBuffer[8192];
		int m_nWriteLen;   
		bool m_bWriteInProgress;
		boost::mutex m_WriteMutex;
		int m_NetId; //记录玩家id或服务器id
		BasePtr m_pHandler;
	};
}


#endif