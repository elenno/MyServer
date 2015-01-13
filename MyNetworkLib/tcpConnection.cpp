#include "tcpConnection.h"
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include "funcHandler.h"
#include "message.h"
#include "log_system.h"

using namespace boost::asio;

my::TcpConnection::TcpConnection(io_service& ios, BasePtr handler) : m_Socket(ios), m_pHandler(handler)
{
	memset(m_WriteBuffer, 0, sizeof(m_WriteBuffer));
	memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));
	m_nWriteLen = 0;
	m_bWriteInProgress = false;
	m_HeartBeat = boost::get_system_time();
	m_nPlayerId = 0;
}

my::TcpConnection::~TcpConnection()
{

}

ip::tcp::socket& my::TcpConnection::getSocket()
{
	return m_Socket;
}

void my::TcpConnection::start()
{
	//m_Socket.non_blocking(true);
	//m_ReadBuffer = boost::shared_ptr<std::string>(new std::string());
	m_Socket.async_read_some(buffer(m_ReadBuffer), 
		boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void my::TcpConnection::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{	
		LogW << "Write Error! Close Socket! " << err.message() << LogEnd;
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
		m_nWriteLen = 0;
		return;
	}
	m_bWriteInProgress = false;

	try{
		boost::mutex::scoped_lock lock(m_WriteMutex);
		LogD << "Write Len = " << m_nWriteLen << LogEnd;
		size_t wlen = m_Socket.write_some(buffer(m_WriteBuffer, m_nWriteLen));
		m_nWriteLen = 0;
		if (wlen == 0)
		{
			//出错
			LogW << "Error wlen! wlen = " << wlen << LogEnd;
			return;
		}	
	}catch(std::exception& e)
	{
		LogE << e.what() << LogEnd;
	}

	//m_Socket.async_read_some(buffer(m_ReadBuffer), boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void my::TcpConnection::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{		
		LogW << "net_id: " << m_NetId << "  reason: " << err.message() << LogEnd;
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}		
		return;
	}
	LogD << "Receive Message: length=" << bytes_transferred << LogEnd; 
	//解包
    NetMessage reqMsg;
    if (!reqMsg.deserialize(m_ReadBuffer, bytes_transferred))
	{
		//解包错误
		LogW << "Deserialize Message Error" << LogEnd;
	}
	else
	{
		LogD << "Receive Message: netId=" << getNetId() << "  length=" << reqMsg.getLen() << "  content=" << reqMsg.getMessage() << LogEnd;
		m_pHandler->onRecv(shared_from_this(), reqMsg);
	}
	post_read();

	//m_Socket.get_io_service().post(boost::bind(&TcpConnection::post_write, shared_from_this()));
	//m_Socket.async_write_some(buffer(m_WriteBuffer), boost::bind(&TcpConnection::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void my::TcpConnection::post_write()
{
	if (m_bWriteInProgress)
	{
		LogW << "WriteInProgress" << LogEnd;
		return;
	}
	m_bWriteInProgress = true;
	m_Socket.async_write_some(boost::asio::null_buffers(), boost::bind(&TcpConnection::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void my::TcpConnection::post_read()
{
	m_Socket.async_read_some(buffer(m_ReadBuffer), boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

int my::TcpConnection::sendMessage(NetMessage& msg)
{
	if (!m_Socket.is_open())
	{
		LogW << "socket is close, netId=" << m_NetId << LogEnd;
		return -1;
	}
	if (msg.getLen() > 8192)
	{
		LogD << "The message is too large to send, len=" << msg.getLen() << "  proto=" << msg.getProto() << "  msg=" << msg.getMessage()  << LogEnd;
		return 1;
	}
	if (m_bWriteInProgress)
	{
		LogD << "Write In Progress, Don't Send This Msg! len=" << msg.getLen() << "  content=" << msg.getMessage() << LogEnd;
		return 2;
	}
	m_nWriteLen = 0;
	memcpy(m_WriteBuffer + m_nWriteLen, msg.getStream(), msg.getLen());
	m_nWriteLen += msg.getLen();
	post_write();
	return 0;
}

void my::TcpConnection::setNetId(int netId)
{
	m_NetId = netId;
}

int my::TcpConnection::getNetId()
{
	return m_NetId;
}

void my::TcpConnection::stop()
{
	try
	{
		m_bWriteInProgress = false;
		memset(m_WriteBuffer, 0, sizeof(m_WriteBuffer));
		memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
	}
	catch(std::exception& e)
	{
		LogE << "Stop Connection Exception: " << e.what() << LogEnd;
	}
}

void my::TcpConnection::setHeartBeat(boost::system_time heartBeat)
{
	m_HeartBeat = heartBeat;
}

boost::system_time my::TcpConnection::getHeartBeat()
{
	return m_HeartBeat;
}

void my::TcpConnection::setPlayerId(int playerId)
{
	m_nPlayerId = playerId;
}

int my::TcpConnection::getPlayerId()
{
	return m_nPlayerId;
}