#include "tcpClient.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp> 
#include <string.h>
#include <sstream>
#include "log_system.h"
#include "json/json.h"
#include "message.h"
#include "helpFunctions.h"

my::TcpClient::TcpClient(boost::asio::io_service& ios, ip::tcp::endpoint& en, std::string prefix, int id) : m_Service(ios), m_Socket(ios), m_EndPoint(en), m_Robot(prefix, id)
{
	m_nWriteLen = 0;
	count = 0;
}

my::TcpClient::~TcpClient()
{

}

void my::TcpClient::run()
{
	try{
		m_Service.run();
	}catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	
}

void my::TcpClient::start()
{
	m_Socket.async_connect(m_EndPoint, boost::bind(&TcpClient::handle_connect, shared_from_this(), 
		boost::asio::placeholders::error));
}

void my::TcpClient::post_connect()
{
    m_Service.post(boost::bind(&TcpClient::start, shared_from_this()));
}

void my::TcpClient::handle_connect(const boost::system::error_code& err)
{
	if (err)
	{
		m_Socket.close();
	}
	try{
	static ip::tcp::no_delay option(true);
	m_Socket.set_option(option);
	m_Robot.reset();
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

void my::TcpClient::post_write()
{
	//just for test
	if (!m_Socket.is_open())
	{
		post_connect();
		return;
	}
	
	Json::Value reqJson;
	int step = -1;
	int playerId = -1;
	m_Robot.doAction(reqJson, step, playerId);
	if (step == -1)
	{
		if (count < 5)
		{
			//todo: 尚未返回，再等
			count++;
			//boost::this_thread::sleep(boost::posix_time::seconds(3));
			//post_write();
		    return;
		}
		else
		{
			//等太久了，重置robot
			m_Robot.reset();
			//boost::this_thread::sleep(boost::posix_time::seconds(3));
			//post_write();
			return;
		}
        
	}
	std::string tmp = my::HelpFunctions::tighten(reqJson.toStyledString());
	NetMessage msg(tmp, step, playerId, 0);
	msg.serialize();
	memcpy(m_WriteBuff, msg.getStream(), msg.getLen());
	m_nWriteLen += msg.getLen();
	
	m_Socket.async_write_some(buffer(m_WriteBuff, m_nWriteLen), boost::bind(&TcpClient::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
	//boost::this_thread::sleep(boost::posix_time::seconds(3));
	//post_write();
}

void my::TcpClient::post_read()
{
	m_Socket.async_read_some(buffer(m_ReadBuff), boost::bind(&TcpClient::handle_read, shared_from_this(), 
			boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred));
}
void my::TcpClient::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	//std::cout << "Send: " << m_WriteBuff << std::endl;
	memset(m_ReadBuff, 0, sizeof(m_ReadBuff));
	LogD << "Send msg: len=" << m_nWriteLen << LogEnd;
	m_nWriteLen = 0;
	m_Service.post(boost::bind(&TcpClient::post_read, shared_from_this()));
	//以下代码是ios.run()下用的代码
	//m_Socket.async_read_some(buffer(m_ReadBuff), boost::bind(&TcpClient::handle_read, shared_from_this(), 
	//	boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred));
}

void my::TcpClient::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{
		if (m_Socket.is_open())
		{
			m_Socket.close();
			
		}
		post_connect();
		return;
	}
	//std::cout << "Receive Message: length=" << bytes_transferred << std::endl;
	NetMessage msg;
	if (!msg.deserialize(m_ReadBuff, bytes_transferred))
	{
		printf("Deserialize Message Failed\n");
	}
	m_Robot.handleMsg(msg);
	post_write();
	//std::cout << "Receive Message: content=" << msg.getMessage() << std::endl;

	//Json::Value reqJson;
	//reqJson["msg"] = 10001;
	//string tmp = helpFunc.tighten(reqJson.toStyledString());
	//NetMessage msg1(reqJson.toStyledString(), api::SHOW_BOOK_LIST_REQ);
	//msg1.serialize();
	//memcpy(m_WriteBuff, msg1.getStream(), msg1.getLen());
	//m_nWriteLen += msg1.getLen();

	//m_Service.post(boost::bind(&TcpClient::post_write, shared_from_this()));

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

void my::TcpClient::stop()
{
	m_Service.stop();
}