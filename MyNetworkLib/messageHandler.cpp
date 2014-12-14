#include "messageHandler.h"
#include "funcHandler.h"

my::MessageHandler::MessageHandler()
{

}

my::MessageHandler::~MessageHandler()
{

}

void my::MessageHandler::onRecv(ConnectionPtr conn, NetMessage& req)
{
	NetMessage rsp;
	funcHandlerMgr.runFuncHandler(req, rsp);
	rsp.serialize();
	conn->sendMessage(rsp);
}