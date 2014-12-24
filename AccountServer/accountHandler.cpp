#include "accountHandler.h"
#include "log_system.h"
#include "protocol.h"
#include "funcHandler.h"

my::AccountHandler::AccountHandler()
{

}

my::AccountHandler::~AccountHandler()
{

}

void my::AccountHandler::onRecv(ConnectionPtr conn, NetMessage& req)
{
    int protoId = req.getProto();
	LogD << "Recv msg: " << req.getMessage() << "  protocol:" << req.getProto() << LogEnd;
	if (protoId <= my::protocol::ACCOUNT_BEGIN || protoId >= my::protocol::ACCOUNT_END)
	{
		LogW << "Unknown protocol, protoId=" << protoId << LogEnd;
		return;
	}
	NetMessage rsp;
	funcHandlerMgr.runFuncHandler(req, rsp);
	rsp.serialize();
	conn->sendMessage(rsp);
}

