#include "gameHandler.h"
#include "tcpConnection.h"
#include "funcHandler.h"
#include "log_system.h"
#include "protocol.h"
#include "gameServer.h"

my::GameHandler::GameHandler()
{

}

my::GameHandler::~GameHandler()
{

}

void my::GameHandler::onRecv(ConnectionPtr conn, NetMessage& req)
{
	int netId = conn->getNetId(); //¿ÉÒÔÅÐ¶ÏnetId

	int protoId = req.getProto();
	int playerId = req.getPlayerId();

	if (protoId > my::protocol::GAME_BEGIN && protoId < my::protocol::GAME_END)
	{
		gameSvr.pushMessage(req);
		//NetMessage rsp;
		//funcHandlerMgr.runFuncHandler(req, rsp);
		//rsp.serialize();
		//conn->sendMessage(rsp);
	}
	else
	{
		LogW << "Unknown Protocol, protoId=" << protoId << " playerId=" << playerId << LogEnd;
	}
}