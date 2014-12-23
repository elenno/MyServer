#include "robot.h"
#include "stringDef.h"
#include "protocol.h"
#include "log_system.h"
#include <boost/lexical_cast.hpp>

my::Robot::Robot(std::string prefix, int id)
{
	init(prefix, id);
}

my::Robot::~Robot()
{

}

Json::Value& my::Robot::getPlayerInfo()
{
	return m_PlayerInfo;
}

Json::Value& my::Robot::getAccountInfo()
{
	return m_AccountInfo;
}

int my::Robot::getStep()
{
    return m_nStep;
}

void my::Robot::setStep(int step)
{
	m_nStep = step;
}

void my::Robot::init(std::string prefix, int id)
{
	std::string username = prefix + boost::lexical_cast<std::string, int>(id);
	m_AccountInfo[db::Account::userName] = username;
	m_AccountInfo[db::Account::userPass] = "123456";
	m_nStep = protocol::PLAYER_LOGIN_REQ;
}

void my::Robot::handleMsg(NetMessage& msg)
{
    Json::Value msgJson;
	Json::Reader reader;
	if (!reader.parse(msg.getMessage(), msgJson))
	{
		LogW << "Can't get the Json msg, username=" << m_AccountInfo[db::Account::userName].asString() << LogEnd;
	    return;
	}
	int protoId = msg.getProto();
	if (protoId == protocol::PLAYER_LOGIN_RSP)
	{
		handleLoginRsp(msgJson);
	}
	else if (protoId == protocol::PLAYER_REGIST_RSP)
	{
		handleRegistRsp(msgJson);
	}
}

void my::Robot::handleLoginReq(Json::Value& msg)
{
	msg["msg"][0u] = m_AccountInfo[db::Account::userName];
	msg["msg"][1u] = m_AccountInfo[db::Account::userPass];

	m_nStep = protocol::PLAYER_LOGIN_RSP;
}

void my::Robot::handleLoginRsp(Json::Value& msg)
{
	int msgId = msg["msg"][0u].asInt();
	if (0 == msgId)
	{
		int playerId = msg["msg"][1u].asInt();
		m_PlayerInfo[db::Player::playerId] = playerId;
		m_nStep = protocol::ENTER_GAME_REQ;
	}
	else if (1 == msgId)
	{
		LogW << "Login Failed Not Regist! username=" << m_AccountInfo[db::Account::userName].asString() << "  result=" << msgId << LogEnd;
		m_nStep = protocol::PLAYER_REGIST_REQ;
	}
    else
	{
		LogW << "Login Failed! username=" << m_AccountInfo[db::Account::userName].asString() << "  result=" << msgId << LogEnd;
	    m_nStep = protocol::PLAYER_LOGIN_REQ;
	}
}

void my::Robot::handleRegistReq(Json::Value& msg)
{
	msg["msg"][0u] = m_AccountInfo[db::Account::userName];
	msg["msg"][1u] = m_AccountInfo[db::Account::userPass];

	m_nStep = protocol::PLAYER_REGIST_RSP;
}

void my::Robot::handleRegistRsp(Json::Value& msg)
{
	int msgId = msg["msg"][0u].asInt();
	if (0 == msgId)
	{
		int playerId = msg["msg"][1u].asInt();
		m_PlayerInfo[db::Player::playerId] = playerId;
		m_nStep = protocol::PLAYER_LOGIN_REQ;
	}
	else
	{
		LogW << "Regist Failed! username=" << m_AccountInfo[db::Account::userName].asString() << "  result=" << msgId << LogEnd;
		m_nStep = protocol::PLAYER_LOGIN_REQ;
	}
}

void my::Robot::doAction(Json::Value& msg, int& step, int& playerId)
{
	step = m_nStep;
	playerId = m_PlayerInfo[db::Player::playerId].asInt();
	if (m_nStep == protocol::PLAYER_LOGIN_REQ)
	{
		handleLoginReq(msg);
	}
	else if (m_nStep == protocol::PLAYER_REGIST_REQ)
	{
		handleRegistReq(msg);
	}
	else
	{
	    //µÈ´ý·µ»Ø
		step = -1;
		return;
	}
}

void my::Robot::reset()
{
	m_nStep = protocol::PLAYER_LOGIN_REQ;
}