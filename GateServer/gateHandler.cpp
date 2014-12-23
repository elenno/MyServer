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
		//server������ 
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
		//��gate����
	}
	else if (protoId > protocol::ACCOUNT_BEGIN && protoId < protocol::ACCOUNT_END)
	{
		//�յ�accountSvr�ķ���
		Json::Value json;
		Json::Reader reader;
		if (reader.parse(msg.getMessage(), json))
		{
			if (protoId == protocol::PLAYER_LOGIN_RSP)
			{
				int errorcode = json["msg"][0u].asInt();
				if (0 == errorcode)
				{
					//��½�ɹ�
					int playerId = json["msg"][1u].asInt();
					gateSvr.onPlayerLogin(playerId);
				}
			}
		}
		gateSvr.sendToPlayer(msg);
	}
	else if (protoId > protocol::GAME_BEGIN && protoId < protocol::GAME_END)
	{
		//�յ�gameSvr�ķ���
	    gateSvr.sendToPlayer(msg);
	}
	else
	{

	}
}

void my::GateHandler::handleClientMsg(ConnectionPtr conn, NetMessage& req)
{
	int protoId = req.getProto();
	if (protoId > my::protocol::GATE_BEGIN && protoId < my::protocol::GATE_END)
	{
		//��ҷ��͸�gate�� ������

	}
	else if (protoId > my::protocol::GAME_BEGIN && protoId < my::protocol::GAME_END)
	{
		//ת����gameSvr
		gateSvr.sendToGameSvr(req);
	}
	else if (protoId > my::protocol::ACCOUNT_BEGIN && protoId < my::protocol::ACCOUNT_END)
	{
		gateSvr.sendToAccountSvr(req);
	}
}