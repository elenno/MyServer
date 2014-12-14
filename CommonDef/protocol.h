#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

namespace my
{
	enum ProtoType
	{
		/*************************************** GateSvr **************************************/
		GATE_BEGIN = 1001,
		GATE_END = 2000,

		/*************************************** GameSvr **************************************/
		GAME_BEGIN = 10001,
		GAME_END = 20000,   

	};
}


#endif