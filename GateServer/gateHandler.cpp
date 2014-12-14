#include "gateHandler.h"
#include "funcHandler.h"
#include "protocol.h"

my::GateHandler::GateHandler()
{

}

my::GateHandler::~GateHandler()
{

}

void my::GateHandler::onRecv(ConnectionPtr conn, NetMessage& req)
{
	if (conn->getNetId() < 0)
	{
		//server������ 
		handleServerMsg(conn, req);
	}

	int protoId = req.getProto();

	if (protoId > my::ProtoType::GAME_BEGIN && protoId < my::ProtoType::GAME_END)
	{
		//ת����gamesvr
		//��ȡgameConnectionPtr,ת����gamesvr
	}

	//NetMessage rsp;
	//funcHandlerMgr.runFuncHandler(req, rsp);
	//rsp.serialize();
	//conn->sendMessage(rsp);
}

void my::GateHandler::handleServerMsg(ConnectionPtr conn, NetMessage& req)
{
	int protoId = req.getProto();
	if (protoId > my::GATE_BEGIN && protoId < my::GATE_END)
	{
		//��gate����
	}
	else
	{
		//���ظ��ͻ���
		req.serialize();
		conn->sendMessage(req);
	}
}