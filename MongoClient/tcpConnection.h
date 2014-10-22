#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
public:
	typedef boost::shared_ptr<TcpConnection> ptr;
	TcpConnection(io_service& ios);
	~TcpConnection();
	
	void start();

	ip::tcp::socket& getSocket();

	void handle_write(const boost::system::error_code& err, size_t bytes_transferred);
	void handle_read(const boost::system::error_code& err, size_t bytes_transferred);

private:
	ip::tcp::socket m_Socket;
	char m_szBuff[1024];
};

#endif