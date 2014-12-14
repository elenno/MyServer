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
		//server传来的 
		handleServerMsg(conn, req);
	}

	int protoId = req.getProto();

	if (protoId > my::ProtoType::GAME_BEGIN && protoId < my::ProtoType::GAME_END)
	{
		//转发到gamesvr
		//获取gameConnectionPtr,转发到gamesvr
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
		//由gate处理
	}
	else
	{
		//返回给客户端
		req.serialize();
		conn->sendMessage(req);
	}
}