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
			GATE_END = 2000,

			/*************************************** AccountSvr ***********************************/
			ACCOUNT_BEGIN = 2001,
			    PLAYER_REGIST_REQ,
				PLAYER_REGIST_RSP,
				PLAYER_LOGIN_REQ,
				PLAYER_LOGIN_RSP,
			ACCOUNT_END = 3000,
			/*************************************** GameSvr **************************************/
			GAME_BEGIN = 10001,

			CREATE_ROLE_REQ,
			CREATE_ROLE_RSP,

			ENTER_GAME_REQ,
			ENTER_GAME_RSP,

			GAME_END = 20000,  
		};
	}
}

#endif