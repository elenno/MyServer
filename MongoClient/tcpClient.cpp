#include "tcpClient.h"
#include <boost/bind.hpp>
#include <string.h>
#include <sstream>
#include "json/json.h"
#include "message.h"
#include "helpFunctions.h"

int TcpClient::count = 0;

TcpClient::TcpClient(boost::asio::io_service& ios, ip::tcp::endpoint& en) : m_Service(ios), m_Socket(ios), m_EndPoint(en)
{
	m_nWriteLen = 0;
}

TcpClient::~TcpClient()
{

}

void TcpClient::run()
{
	try{
		m_Service.run();
	}catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	
}

void TcpClient::start()
{
	m_Socket.async_connect(m_EndPoint, boost::bind(&TcpClient::handle_connect, shared_from_this(), 
		boost::asio::placeholders::error));
}

void TcpClient::post_connect()
{
    m_Service.post(boost::bind(&TcpClient::start, shared_from_this()));
}

void TcpClient::handle_connect(const boost::system::error_code& err)
{
	if (err)
	{
		m_Socket.close();
	}
	try{
	static ip::tcp::no_delay option(true);
	m_Socket.set_option(option);
	m_Service.post(boost::bind(&TcpClient::post_write, shared_from_this()));
    }catch(std::exception& e)
    {
	    std::cout << e.what() << std::endl;
    }
	//以下代码是ios.run()下用的代码
	//std::stringstream s;
	//s << "Hello World: " << TcpClient::count;
	//memset(m_WriteBuff, 0, sizeof(m_WriteBuff));
	//strcpy(m_WriteBuff, s.str().c_str());
	//TcpClient::count ++;
	//m_Socket.async_write_some(buffer(m_WriteBuff), boost::bind(&TcpClient::handle_write, shared_from_this(),
	//	boost::asio::placeholders::error,
	//	boost::asio::placeholders::bytes_transferred));
}

void TcpClient::post_write()
{
	//just for test
	
	Json::Value reqJson;
	reqJson["msg"] = 10001;
	string tmp = helpFunc.tighten(reqJson.toStyledString());
	NetMessage msg(reqJson.toStyledString(), api::SHOW_BOOK_LIST_REQ);
	msg.serialize();
	memcpy(m_WriteBuff, msg.getStream(), msg.getLen());
	m_nWriteLen += msg.getLen();
	TcpClient::count ++;

	if (TcpClient::count > 250000)
	{
		printf("already 100 times\n");
		m_Socket.close();
		return;
	}
	
	m_Socket.async_write_some(buffer(m_WriteBuff, m_nWriteLen), boost::bind(&TcpClient::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void TcpClient::post_read()
{
	m_Socket.async_read_some(buffer(m_ReadBuff), boost::bind(&TcpClient::handle_read, shared_from_this(), 
			boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred));
}
void TcpClient::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	//std::cout << "Send: " << m_WriteBuff << std::endl;
	memset(m_ReadBuff, 0, sizeof(m_ReadBuff));
	m_nWriteLen = 0;
	m_Service.post(boost::bind(&TcpClient::post_read, shared_from_this()));
	//以下代码是ios.run()下用的代码
	//m_Socket.async_read_some(buffer(m_ReadBuff), boost::bind(&TcpClient::handle_read, shared_from_this(), 
	//	boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred));
}

void TcpClient::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{
		m_Socket.close();
	}
	//std::cout << "Receive Message: length=" << bytes_transferred << std::endl;
	NetMessage msg;
	if (!msg.deserialize(m_ReadBuff, bytes_transferred))
	{
		printf("Deserialize Message Failed\n");
	}
	//std::cout << "Receive Message: content=" << msg.getMessage() << std::endl;

	//Json::Value reqJson;
	//reqJson["msg"] = 10001;
	//string tmp = helpFunc.tighten(reqJson.toStyledString());
	//NetMessage msg1(reqJson.toStyledString(), api::SHOW_BOOK_LIST_REQ);
	//msg1.serialize();
	//memcpy(m_WriteBuff, msg1.getStream(), msg1.getLen());
	//m_nWriteLen += msg1.getLen();

	m_Service.post(boost::bind(&TcpClient::post_write, shared_from_this()));

	//以下代码是ios.run()下用的代码
	//std::stringstream s;
	//s << "Hello World: " << TcpClient::count;
	//memset(m_WriteBuff, 0, sizeof(m_WriteBuff));
	//strcpy(m_WriteBuff, s.str().c_str());
	//TcpClient::count ++;
	//m_Socket.async_write_some(buffer(m_WriteBuff, m_nWriteLen), boost::bind(&TcpClient::handle_write, shared_from_this(),
	//	boost::asio::placeholders::error,
	//	boost::asio::placeholders::bytes_transferred));
}

void TcpClient::stop()
{
	m_Service.stop();
}