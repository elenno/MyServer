#ifndef __PLAYER_MGR_H__ 
#define __PLAYER_MGR_H__

#include "../common/common.h"
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

		void newPlayer(int playerId);
		bool findPlayer(int playerId, Json::Value& json);
		bool modifyUpdatePlayerInfo(int playerId, Json::Value& json);


	private:
		typedef std::map<int, Json::Value> PlayerInfoMap;
		PlayerInfoMap m_PlayerInfoMap;
		typedef std::map<std::string, int> NickNameMap;
		NickNameMap m_NickNameMap;

		bool savePlayer(int playerId, Json::Value& json);
	};
}

#endif