#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

namespace my
{
	namespace protocol
	{
		enum
		{
			/*************************************** GateSvr **************************************/
			GATE_BEGIN = 1001,
			    PLAYER_HEART_BEAT_REQ,
				PLAYER_HEART_BEAT_RSP,
			GATE_END = 2000,

			/*************************************** AccountSvr ***********************************/
			ACCOUNT_BEGIN = 2001,
			    PLAYER_REGIST_REQ,
				PLAYER_REGIST_RSP,
				PLAYER_LOGIN_REQ,
				PLAYER_LOGIN_RSP,
			ACCOUNT_END = 3000,

			/*************************************** MysqlSvr ***********************************/
			MYSQL_BEGIN = 3001,
			MYSQL_END = 4000,
			/*************************************** GameSvr **************************************/
			GAME_BEGIN = 10001,
			    PLAYER_CREATE_ROLE_REQ,
			    PLAYER_CREATE_ROLE_RSP,
			    PLAYER_ENTER_GAME_REQ,
			    PLAYER_ENTER_GAME_RSP,
			GAME_END = 20000,  
		};
	}
}

#endif