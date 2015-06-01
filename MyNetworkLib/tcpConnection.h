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
	class TcpServer;
	class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
	{
	public:
		typedef boost::shared_ptr<TcpServer> ServerPtr;
		typedef boost::shared_ptr<BaseHandler> BasePtr;
		TcpConnection(io_service& ios, BasePtr handlerPtr, ServerPtr serverPtr);
		~TcpConnection();

		ip::tcp::socket& getSocket();
		void setNetId(int netId);
		int getNetId();
		void setHeartBeat(boost::system_time heartBeat);
		boost::system_time getHeartBeat();
		void setPlayerId(int playerId);
		int getPlayerId();

		int sendMessage(NetMessage& msg);
		void start();
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
		bool m_bReadInProgress;
		boost::mutex m_ReadMutex;
		boost::mutex m_WriteMutex;
		int m_nNetId; //记录玩家id或服务器id
		BasePtr m_pHandler;
		boost::system_time m_HeartBeat;
		int m_nPlayerId;
		ServerPtr m_pServer;
	};
}


#endif