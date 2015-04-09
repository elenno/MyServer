#ifndef __PLAYER_MGR_H__ 
#define __PLAYER_MGR_H__

#include "json/json.h"
#include <boost/thread/detail/singleton.hpp>

#define playerMgr boost::detail::thread::singleton<my::PlayerMgr>::instance()

namespace my
{
	class PlayerMgr
	{
	public:
		PlayerMgr();
		~PlayerMgr();

		//client functions
		bool createRoleReq(Json::Value& req, Json::Value& rsp, int playerId);
		bool enterGameReq(Json::Value& req, Json::Value& rsp, int playerId);

		bool newPlayer(int playerId, std::string nickName);
		bool findPlayer(int playerId, Json::Value& json);
		bool modifyUpdatePlayerInfo(int playerId, Json::Value& json);

	private:
		bool savePlayer(int playerId, Json::Value& json);
		void regFunc();

	private:
		typedef std::map<int, Json::Value> PlayerInfoMap;
		PlayerInfoMap m_PlayerInfoMap;
		typedef std::map<std::string, int> NickNameMap;
		NickNameMap m_NickNameMap;

		
	};
}

#endif