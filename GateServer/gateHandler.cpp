#include "gateHandler.h"
#include "funcHandler.h"
#include "protocol.h"
#include "gateServer.h"
#include "log_system.h"
#include <json/json.h>

my::GateHandler::GateHandler()
{

}

my::GateHandler::~GateHandler()
{

}

void my::GateHandler::onRecv(ConnectionPtr conn, NetMessage& msg)
{
	if (conn->getNetId() < 0)
	{
		//server传来的 
		handleServerMsg(conn, msg);
	}
	else
	{
		handleClientMsg(conn, msg);
	}
}

void my::GateHandler::handleServerMsg(ConnectionPtr conn, NetMessage& msg)
{
	int protoId = msg.getProto();
	if (protoId > protocol::GATE_BEGIN && protoId < protocol::GATE_END)
	{
		
	}
	else if (protoId > protocol::ACCOUNT_BEGIN && protoId < protocol::ACCOUNT_END)
	{
		//收到accountSvr的返回
		Json::Value json;
		Json::Reader reader;
		if (reader.parse(msg.getMessage(), json))
		{
			if (protoId == protocol::PLAYER_LOGIN_RSP)
			{
				int errorcode = json["msg"][0u].asInt();
				if (0 == errorcode)
				{
					//登陆成功
					int playerId = json["msg"][1u].asInt();
					int netId = msg.getNetId();
					gateSvr.onPlayerLogin(playerId, netId);
					msg.setPlayerId(playerId);
				}
			}
		}
		gateSvr.sendToPlayer(msg);
	}
	else if (protoId > protocol::GAME_BEGIN && protoId < protocol::GAME_END)
	{
		//收到gameSvr的返回
	    gateSvr.sendToPlayer(msg);
	}
	else
	{

	}
}

void my::GateHandler::handleClientMsg(ConnectionPtr conn, NetMessage& req)
{
	int protoId = req.getProto();
	req.setNetId(conn->getNetId());
	//todo 检查client有效性

	if (protoId > my::protocol::GATE_BEGIN && protoId < my::protocol::GATE_END)
	{
		if (protoId == protocol::PLAYER_HEART_BEAT_REQ)
		{
			conn->setHeartBeat(gateSvr.getSystemTime());
			std::string ret;
			NetMessage rsp(ret, protocol::PLAYER_HEART_BEAT_RSP, req.getPlayerId(), req.getNetId());
			gateSvr.sendToPlayer(rsp);
		}
	}
	else if (protoId > my::protocol::GAME_BEGIN && protoId < my::protocol::GAME_END)
	{
		//转发到gameSvr
		gateSvr.sendToGameSvr(req);
	}
	else if (protoId > my::protocol::ACCOUNT_BEGIN && protoId < my::protocol::ACCOUNT_END)
	{
		gateSvr.sendToAccountSvr(req);
	}
}