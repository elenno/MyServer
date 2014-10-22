#include "../head/tcpConnection.h"
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include "../head/funcHandler.h"
#include "../head/message.h"
#include "../common/log_system.h"

using namespace boost::asio;

TcpConnection::TcpConnection(io_service& ios) : m_Socket(ios)
{
	memset(m_WriteBuffer, 0, sizeof(m_WriteBuffer));
	memset(m_ReadBuffer, 0, sizeof(m_ReadBuffer));
	m_nWriteLen = 0;
	m_bWriteInProgress = false;
}

TcpConnection::~TcpConnection()
{

}

ip::tcp::socket& TcpConnection::getSocket()
{
	return m_Socket;
}

void TcpConnection::start()
{
	//m_Socket.non_blocking(true);
	//m_ReadBuffer = boost::shared_ptr<std::string>(new std::string());
	m_Socket.async_read_some(buffer(m_ReadBuffer), 
		boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{
		printf("Write Error! Close Socket!\n");
		m_Socket.close();
		return;
	}
	m_bWriteInProgress = false;

	try{
		boost::mutex::scoped_lock lock(m_WriteMutex);
		size_t wlen = m_Socket.write_some(buffer(m_WriteBuffer, m_nWriteLen));
		if (wlen == 0)
		{
			//出错
			printf("Error wlen! wlen = %d\n", wlen);
			return;
		}
		m_nWriteLen = 0;
	}catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		LogW << e.what() << LogEnd;
	}

	//m_Socket.async_read_some(buffer(m_ReadBuffer), boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{		
		m_Socket.close();
		return;
	}
	std::cout << "Receive Message: length=" << bytes_transferred <<std::endl; 
	//解包
    NetMessage reqMsg;
    if (!reqMsg.deserialize(m_ReadBuffer, bytes_transferred))
	{
		//解包错误
		printf("Deserialize Message Error\n");
	}
	else
	{
		std::cout << "Receive Message: content=" << reqMsg.getMessage() << std::endl;
		NetMessage rspMsg;
		funcHandlerMgr.runFuncHandler(reqMsg, rspMsg);
		rspMsg.serialize();
		sendMessage(rspMsg);
	}
	post_read();

	//m_Socket.get_io_service().post(boost::bind(&TcpConnection::post_write, shared_from_this()));
	//m_Socket.async_write_some(buffer(m_WriteBuffer), boost::bind(&TcpConnection::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::post_write()
{
	if (m_bWriteInProgress)
	{
		printf("WriteInProgress\n");
		return;
	}
	m_bWriteInProgress = true;
	m_Socket.async_write_some(boost::asio::null_buffers(), boost::bind(&TcpConnection::handle_write, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpConnection::post_read()
{
	m_Socket.async_read_some(buffer(m_ReadBuffer), boost::bind(&TcpConnection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

bool TcpConnection::sendMessage(NetMessage& msg)
{
	if (msg.getLen() > 8192)
	{
		printf("The message is too large to send, len=%d msg=%s\n", msg.getLen(), msg.getStream());
		return false;
	}
	memcpy(m_WriteBuffer + m_nWriteLen, msg.getStream(), msg.getLen());
	m_nWriteLen += msg.getLen();
	post_write();
	return true;
}

