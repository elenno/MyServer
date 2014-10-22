#include "../head/playerMgr.h"
#include "../head/mongoMgr.h"
#include "../head/helpFunctions.h"
#include "../common/log_system.h"

PlayerMgr::PlayerMgr()
{
	m_PlayerInfoMap.clear();
	m_NickNameMap.clear();  
	//read all players' data
}

PlayerMgr::~PlayerMgr()
{
	m_PlayerInfoMap.clear();
	m_NickNameMap.clear();
}

void PlayerMgr::newPlayer(int playerId)
{
	if (m_PlayerInfoMap.find(playerId) != m_PlayerInfoMap.end())
	{
		//error
		return;
	}
	Json::Value player;
	player[db::Player::playerId] = playerId;
	player[db::Player::nickName] = "";
	player[db::Player::level] = 1;
	player[db::Player::vip] = 0;
	m_PlayerInfoMap.insert(std::make_pair<int, Json::Value>(playerId, player));
}

bool PlayerMgr::findPlayer(int playerId, Json::Value& json)
{
	PlayerInfoMap::iterator it = m_PlayerInfoMap.find(playerId);
	if (it == m_PlayerInfoMap.end())
	{
		//�����ݿ�����
		Json::Value key;
		key[db::Player::playerId] = playerId;
		Json::Value player = mongoMgr.findJson(db::Player::colName, key);
		if (player == Json::nullValue)
		{
			//�����ݿⶼû�У��Ķ����˰ɣ�
			LogW << "Can't find player: playerId=" << playerId << LogEnd;
			return false;
		}
		//�ҵ������map��
		m_PlayerInfoMap.insert(std::make_pair<int, Json::Value>(playerId, player));
	    json = player;
	}
	else
	{
		json = it->second;
	}
	return true;
}

bool PlayerMgr::modifyUpdatePlayerInfo(int playerId, Json::Value& json)
{
	m_PlayerInfoMap[playerId] = json;
	//save to db
	if (!savePlayer(playerId, json))
	{
		return false;
	}
	return true;
}

bool PlayerMgr::savePlayer(int playerId, Json::Value& json)
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

