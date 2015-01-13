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
	std::string msgStr = msg.getMessage();
	if (msgStr != "" && !reader.parse(msgStr, msgJson))
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
	else if (protoId == protocol::PLAYER_CREATE_ROLE_RSP)
	{
		handleCreateRoleRsp(msgJson);
	}
	else if (protoId == protocol::PLAYER_ENTER_GAME_RSP)
	{
		handleEnterGameRsp(msgJson);
	}
	else if (protoId == protocol::PLAYER_HEART_BEAT_RSP)
	{
		handleHeartBeatRsp(msgJson);
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
		m_nStep = protocol::PLAYER_ENTER_GAME_REQ;
		//m_nStep = protocol::PLAYER_LOGIN_REQ;
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

void my::Robot::handleEnterGameReq(Json::Value& msg)
{
	m_nStep = protocol::PLAYER_ENTER_GAME_RSP;
}

void my::Robot::handleEnterGameRsp(Json::Value& msg)
{
    int msgId = msg["msg"][0u].asInt();
	if (0 == msgId)
	{
		Json::Value& info = msg["msg"][1u];
		m_PlayerInfo[db::Player::nickName] = info[db::Player::nickName];
		m_PlayerInfo[db::Player::level] = info[db::Player::level];
		m_PlayerInfo[db::Player::vip] = info[db::Player::vip];
		m_nStep = protocol::PLAYER_HEART_BEAT_REQ;
	}
	else if (1 == msgId)
	{
		m_nStep = protocol::PLAYER_CREATE_ROLE_REQ;
	}
	else
	{
		m_nStep = protocol::PLAYER_LOGIN_REQ;
	}
}

void my::Robot::handleCreateRoleReq(Json::Value& msg)
{
	msg["msg"][0u] = m_AccountInfo[db::Account::userName]; //昵称直接等于用户名
	m_nStep = protocol::PLAYER_CREATE_ROLE_REQ;
}

void my::Robot::handleCreateRoleRsp(Json::Value& msg)
{
	int msgId = msg["msg"][0u].asInt();
	if (0 == msgId)
	{
		m_nStep = protocol::PLAYER_ENTER_GAME_REQ;
	}
	else
	{
		m_nStep = protocol::PLAYER_LOGIN_REQ;
	}
}

void my::Robot::handleHeartBeatReq(Json::Value& msg)
{
	m_nStep = protocol::PLAYER_HEART_BEAT_REQ;
}

void my::Robot::handleHeartBeatRsp(Json::Value& msg)
{
	m_nStep = protocol::PLAYER_LOGIN_REQ;

}

void my::Robot::doAction(Json::Value& msg, int& step, int& playerId)
{
	step = m_nStep;
	playerId = m_PlayerInfo[db::Player::playerId].asInt();
	if (m_nStep == protocol::PLAYER_LOGIN_REQ)
	{
		LogD << "Player Request Login" << LogEnd;
		handleLoginReq(msg);
	}
	else if (m_nStep == protocol::PLAYER_REGIST_REQ)
	{
		LogD << "Player Request Regist" << LogEnd;
		handleRegistReq(msg);
	}
	else if (m_nStep == protocol::PLAYER_CREATE_ROLE_REQ)
	{
		LogD << "Player Request CreateRole" << LogEnd;
		handleCreateRoleReq(msg);
	}
	else if (m_nStep == protocol::PLAYER_ENTER_GAME_REQ)
	{
		LogD << "Player Request EnterGame" << LogEnd;
		handleEnterGameReq(msg);
	}
	else if (m_nStep == protocol::PLAYER_HEART_BEAT_REQ)
	{
		LogD << "Player Request HeartBeat" << LogEnd;
		handleHeartBeatReq(msg);
	}
	else
	{
	    //等待返回
		step = -1;
		return;
	}
}

void my::Robot::reset()
{
	m_nStep = protocol::PLAYER_LOGIN_REQ;
}

std::string my::Robot::getUserName()
{
	return m_AccountInfo[db::Account::userName].asString();
}