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
#include "stringDef.h"

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
		std::cout << err.message() << std::endl;
		m_Socket.close();
		std::cout << "socket closed! reconnect!" << std::endl;
		post_connect();
		return;
	}
	LogI << "connect ok!! name=" << m_Robot.getAccountInfo()[db::Account::userName].asString() << LogEnd;
	std::cout << "connect ok!" << " name=" << m_Robot.getAccountInfo()[db::Account::userName].asString() << std::endl;
	try{
	static ip::tcp::no_delay option(true);
	m_Socket.set_option(option);
	m_Robot.reset();
	
	post_read();
	do_some_thing();
	//post_write();
	//m_Service.post(boost::bind(&TcpClient::post_write, shared_from_this()));
    }catch(std::exception& e)
    {
	    std::cout << e.what() << std::endl;
    }
}

void my::TcpClient::post_write()
{
	//just for test
	if (!m_Socket.is_open())
	{
		LogW << "socket closed! reconnect!" << LogEnd;
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
			LogW << "No Response Yet! Wait" << LogEnd;
		    return;
		}
		else
		{
			//等太久了，重置robot
			count = 0;
			m_Robot.reset();
			LogW << "Reset Robot!!!" << LogEnd;
			return;
		}     
	}
	//if (step == 1002)
	//{
	//	LogD << "Sleeping..." << LogEnd;
	//	HelpFunctions::threadSleepSecond(5);//沉睡一段时间
	//}
	//HelpFunctions::threadSleepSecond(1);
	//std::string tmp = "GET /www/br_temp/t546 HTTP/1.1\r\n";
	//tmp += "Host: 127.0.0.1\r\n";
	//tmp += "Connection: keep-alive\r\n";
	//tmp += "Cache-Control: max-age=0\r\n";
	//tmp += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
	//tmp += "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/33.0.1750.154 Safari/537.36\r\n";
	//tmp += "Accept-Encoding: gzip,deflate,sdch\r\n";
	//tmp += "Accept-Language: zh-CN,zh;q=0.8\r\n";
	//tmp += "Cookie: rock_format=json\r\n";
	//tmp += "\r\n";
	std::string tmp = util::HelpFunctions::tighten(reqJson.toStyledString());
	NetMessage msg(tmp, step, playerId, 0);
	msg.serialize();
	LogD << "send msg: protoId=" << msg.getProto() << " content=" << msg.getMessage() << " msgLen=" << msg.getLen() << LogEnd;
	m_nWriteLen = 0;
	memcpy(m_WriteBuff, msg.getStream(), msg.getLen());
	m_nWriteLen += msg.getLen();
//	memcpy(m_WriteBuff, tmp.c_str(), tmp.length());
//	m_nWriteLen += tmp.length();
	
	m_Socket.async_write_some(buffer(m_WriteBuff, m_nWriteLen), boost::bind(&TcpClient::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
	
}

void my::TcpClient::post_read()
{
	if (!m_Socket.is_open())
	{
        LogW << "socket closed! reconnect!" << LogEnd;
		post_connect();
		return;
	}
	memset(m_ReadBuff, 0, sizeof(m_ReadBuff));
	m_Socket.async_read_some(buffer(m_ReadBuff), boost::bind(&TcpClient::handle_read, shared_from_this(), 
			boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred));
}
void my::TcpClient::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	//LogD << "Send msg: len=" << m_nWriteLen << LogEnd;
	LogD << "Send msg: len=" << m_nWriteLen << " name=" << m_Robot.getAccountInfo()[db::Account::userName].asString() << LogEnd;
	memset(m_WriteBuff, 0, sizeof(m_WriteBuff));
	m_nWriteLen = 0;
	if (err)
	{
		if (m_Socket.is_open())
		{
	        m_Socket.close();
		}
		//LogW << "some error occur:" << err.message() << " Socket closed! reconnect!" << LogEnd;
		LogW << "socket closed! reconnect!" << LogEnd;
		post_connect();
	}
	

}

void my::TcpClient::handle_read(const boost::system::error_code& err, size_t bytes_transferred)
{
	if (err)
	{
		//LogW << "some error occur:" << err.message() << " Socket closed! reconnect!" << LogEnd;
		if (m_Socket.is_open())
		{
			m_Socket.close();
		}
		LogW << "socket closed! reconnect!" << LogEnd;
		post_connect();
		return;
	}

	//std::string tmp(m_ReadBuff, bytes_transferred);
	//std::cout << tmp << std::endl;
	//LogD << "Receive Message: name= " << m_Robot.getPlayerInfo()[db::Account::userName].asString() << " length=" << bytes_transferred << LogEnd;
	NetMessage msg;
	if (!msg.deserialize(m_ReadBuff, bytes_transferred))
	{
		LogW << "Deserialize Message Failed" << LogEnd; 
	}
	LogD << "Receive Message: Name=" << m_Robot.getAccountInfo()[db::Account::userName] << " Msg=" << msg.getMessage() << " Protocol=" << msg.getProto() << LogEnd;
	m_Robot.handleMsg(msg);
	memset(m_ReadBuff, 0, sizeof(m_ReadBuff));

	//do_some_thing();
	post_read();
	//post_write();
	//std::cout << "Receive Message: content=" << msg.getMessage() << std::endl;

}

void my::TcpClient::stop()
{
	m_Service.stop();
}

void my::TcpClient::do_some_thing()
{
	//std::cout << "do some thing!!!" << std::endl;
	LogD << "do some thing!!!" << LogEnd;
	post_write();
	
	util::HelpFunctions::threadSleepSecond(2);
	m_Service.post(boost::bind(&TcpClient::do_some_thing, shared_from_this()));
}