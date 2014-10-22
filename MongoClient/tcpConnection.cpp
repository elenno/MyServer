#include "tcpConnection.h"

TcpConnection::TcpConnection(io_service& ios) : m_Socket(ios)
{
	memset(m_szBuff, 0, sizeof(m_szBuff));
}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::start()
{
	async_write(m_Socket, buffer(m_szBuff), boost::bind(&TcpConnection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred));
}

void TcpConnection::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{

}

void TcpConnection::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{

}