#include "tcpConnection.h"
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <string.h>
#include "funcHandler.h"
#include "message.h"
#include "log_system.h"
#include "core.h"

using namespace boost::asio;

my::TcpConnection::TcpConnection(io_service& ios, BasePtr handlerPtr, ServerPtr serverPtr) : m_Socket(ios), m_pHandler(handlerPtr), m_pServer(serverPtr)
{
	memset(m_WriteBuffer, 0, sizeof(m_WriteBuffer));
	memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));
	m_nWriteLen = 0;
	m_bWriteInProgress = false;
	m_bReadInProgress = false;
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
	post_read();
	//m_Socket.async_read_some(buffer(m_ReadBuffer), 
	//	boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void my::TcpConnection::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	m_bWriteInProgress = false;
	if (!err)
	{
		boost::mutex::scoped_lock lock(m_WriteMutex);
		try{
			LogD << "Write Len = " << m_nWriteLen << LogEnd;
			std::size_t len = 0;
			boost::system::error_code errrr;
			do 
			{
				size_t wlen = m_Socket.write_some(buffer(m_WriteBuffer + len, m_nWriteLen), errrr);
				if (wlen == 0)
				{
					LogE << "wlen == 0, reason=" << errrr.message() << " net_id=" << getNetId() << LogEnd;
					// copy unwritten data to top
					memmove(m_WriteBuffer, m_WriteBuffer + len, m_nWriteLen);
					break;
				}
				len += wlen;
				m_nWriteLen -= wlen;
			} while (m_nWriteLen > 0);
			m_nWriteLen = 0;
		}catch(std::exception& e)
		{
			LogE << "Caught Exception: netId=" << m_nNetId << "  reason=" << e.what() << LogEnd;
		}
	}
	else
	{
		if (err != boost::asio::error::eof)
		{	
			LogE << "Write Error! netId=" << m_nNetId << " reason=" << err.message() << LogEnd;
			m_nWriteLen = 0;
			core.getService().post(boost::bind(&TcpServer::handle_disconnect, m_pServer, shared_from_this()));
			return;
		}
	}
}
void my::TcpConnection::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
	m_bReadInProgress = false;
	if (err)
	{		
		LogW << "net_id: " << m_nNetId << "  reason: " << err.message() << LogEnd;
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
		return;
	}
	//LogD << "Receive Message: length=" << bytes_transferred << LogEnd;
	boost::system::error_code errrr;
	try{
		boost::mutex::scoped_lock lock(m_ReadMutex);
		int total_len = m_Socket.read_some(buffer(m_ReadBuffer, MSG_MAXIMUM), errrr);
		if (!errrr)
		{
			int offset = 0;
			while(total_len > 0)
			{
				NetMessage reqMsg;
				int res = reqMsg.deserialize(m_ReadBuffer + offset, total_len);
				if (0 == res)
				{
					//解包错误
					LogW << "Deserialize Message Error" << LogEnd;
					//可以考虑踢掉客户端
					break;
				}
				else
				{
					total_len -= res;
					offset += res;
					LogD << "Receive Message: netId=" << getNetId() << "  length=" << reqMsg.getLen() << "  content=" << reqMsg.getMessage() << LogEnd;
					m_pHandler->onRecv(shared_from_this(), reqMsg);
				}
			}
		}	
	}
	catch (std::exception& e)
	{
		LogE << "Caught Exception: netId=" << m_nNetId << "  reason=" << e.what() << LogEnd;
	}
	post_read();
}

void my::TcpConnection::post_write()
{
	try{
		if (!m_Socket.is_open() || m_bWriteInProgress ||  0 == m_nWriteLen)
		{
			LogD << "socket is closed || write in progress || data length is 0" << LogEnd;
			return;
		}
		m_bWriteInProgress = true;
		m_Socket.async_write_some(boost::asio::null_buffers(), boost::bind(&TcpConnection::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}catch (std::exception& e)
	{
		LogE << "Caught Exception: netId=" << m_nNetId << "  reason=" << e.what() << LogEnd;
	}
}

void my::TcpConnection::post_read()
{
	try
	{
		if (!m_Socket.is_open() || m_bReadInProgress)
		{
			LogD << "socket is closed || read in progress || data length is 0" << LogEnd;
			return;
		}
		m_bReadInProgress = true;
		m_Socket.async_read_some(boost::asio::null_buffers(), boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}catch (std::exception& e)
	{
		LogE << "Caught Exception: netId=" << m_nNetId << "  reason=" << e.what() << LogEnd;
	}
}

int my::TcpConnection::sendMessage(NetMessage& msg)
{
	if (msg.getLen() > 8192)
	{
		LogD << "The message is too large to send, len=" << msg.getLen() << "  proto=" << msg.getProto() << "  msg=" << msg.getMessage()  << LogEnd;
		return 1;
	}
	if (!m_Socket.is_open())
	{
		LogW << "socket is close, netId=" << m_nNetId << LogEnd;
		return -1;
	}
	if (m_bWriteInProgress)
	{
		LogD << "Write In Progress, Don't Send This Msg! len=" << msg.getLen() << "  content=" << msg.getMessage() << LogEnd;
		return 2;
	}
	{
		boost::mutex::scoped_lock lock(m_WriteMutex);
		m_nWriteLen = 0;
		memcpy(m_WriteBuffer, msg.getStream(), msg.getLen());
		m_nWriteLen += msg.getLen();
	}
	post_write();
	return 0;
}

void my::TcpConnection::setNetId(int netId)
{
	m_nNetId = netId;
}

int my::TcpConnection::getNetId()
{
	return m_nNetId;
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
		m_bWriteInProgress = false;
		m_bReadInProgress = false;
	}
	catch(std::exception& e)
	{
		LogE << "Stop Connection Exception: netId=" << m_nNetId << "  reason=" << e.what() << LogEnd;
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