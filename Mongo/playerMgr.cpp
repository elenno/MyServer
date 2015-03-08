#include "playerMgr.h"
#include "mongoMgr.h"
#include "helpFunctions.h"
#include "log_system.h"
#include "funcHandler.h"
#include "stringDef.h"
#include "protocol.h"
#include "gameServer.h"


my::PlayerMgr::PlayerMgr()
{
	m_PlayerInfoMap.clear();
	m_NickNameMap.clear();  
	//read all players' data

	regFunc();
}

my::PlayerMgr::~PlayerMgr()
{
	m_PlayerInfoMap.clear();
	m_NickNameMap.clear();
}

void my::PlayerMgr::regFunc()
{
	RegistFunc(my::protocol::PLAYER_CREATE_ROLE_REQ, my::protocol::PLAYER_CREATE_ROLE_RSP, PlayerMgr::createRoleReq);
	RegistFunc(my::protocol::PLAYER_ENTER_GAME_REQ, my::protocol::PLAYER_ENTER_GAME_RSP, PlayerMgr::enterGameReq);
}

bool my::PlayerMgr::newPlayer(int playerId, std::string nickname)
{
	if (m_PlayerInfoMap.find(playerId) != m_PlayerInfoMap.end())
	{
		//error
		return false;
	}
	if (m_NickNameMap.find(nickname) != m_NickNameMap.end())
	{
		//error
		return false;
	}
	Json::Value player;
	player[db::Player::playerId] = playerId;
	player[db::Player::nickName] = nickname;
	player[db::Player::level] = 1;
	player[db::Player::vip] = 0;
	m_PlayerInfoMap.insert(std::make_pair<int, Json::Value>(playerId, player));
    savePlayer(playerId, player);
	return true;
}

bool my::PlayerMgr::findPlayer(int playerId, Json::Value& json)
{
	PlayerInfoMap::iterator it = m_PlayerInfoMap.find(playerId);
	if (it == m_PlayerInfoMap.end())
	{
		//从数据库中找
		Json::Value key;
		key[db::Player::playerId] = playerId;
		Json::Value player = mongoMgr.findJson(db::Player::colName, key);
		if (player == Json::nullValue)
		{
			//连数据库都没有，哪儿错了吧？
			LogW << "Player doesn't exist: playerId=" << playerId << LogEnd;
			return false;
		}
		//找到后插入map中
		m_PlayerInfoMap.insert(std::make_pair<int, Json::Value>(playerId, player));
	    json = player;
	}
	else
	{
		json = it->second;
	}
	return true;
}

bool my::PlayerMgr::modifyUpdatePlayerInfo(int playerId, Json::Value& json)
{
	m_PlayerInfoMap[playerId] = json;
	//save to db
	if (!savePlayer(playerId, json))
	{
		return false;
	}
	return true;
}

bool my::PlayerMgr::savePlayer(int playerId, Json::Value& json)
{
	Json::Value key;
	key[db::Player::playerId] = playerId;
	json[db::Player::playerId] = playerId;
	if (!(mongoMgr.saveJson2(db::Player::colName, key, json)))
	{
		return false;
	}
	return true;
}

bool my::PlayerMgr::createRoleReq(Json::Value& req, Json::Value& rsp, int playerId)
{
	if (!req["msg"].isArray() || req["msg"].size() < 1)
	{
		//参数个数不对
		rsp["msg"][0u] = -1;
	    return false;
	}
	if (!req["msg"][0u].isString())
	{
		//参数格式不对
		rsp["msg"][0u] = -1;
		return false;
	}

	std::string nickname = req["msg"][0u].asString();
	newPlayer(playerId, nickname);
	
	rsp["msg"][0u] = 0;
	return true;
}

bool my::PlayerMgr::enterGameReq(Json::Value& req, Json::Value& rsp, int playerId)
{
	Json::Value playerInfo;
	if (!findPlayer(playerId, playerInfo))
	{
		rsp["msg"][0u] = 1; //找不到该用户
		return true;
	}
	rsp["msg"][0u] = 0;
	rsp["msg"][1u][db::Player::nickName] = playerInfo[db::Player::nickName];
	rsp["msg"][1u][db::Player::vip] = playerInfo[db::Player::vip];
	rsp["msg"][1u][db::Player::level] = playerInfo[db::Player::level];
	return true;
}