#include "../head/gameHandler.h"
#include "tcpConnection.h"
#include "funcHandler.h"
#include "log_system.h"
#include "protocol.h"

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
		NetMessage rsp;
		funcHandlerMgr.runFuncHandler(req, rsp);
		rsp.serialize();
		conn->sendMessage(rsp);
	}
	else
	{
		LogW << "Unknown Protocol, protoId=" << protoId << " playerId=" << playerId << LogEnd;
	}
}