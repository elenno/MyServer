#include "mysqlHandler.h"
#include "mysqlServer.h"
#include "log_system.h"
#include "protocol.h"
#include "funcHandler.h"

my::MysqlHandler::MysqlHandler()
{

};

my::MysqlHandler::~MysqlHandler()
{

}

void my::MysqlHandler::onRecv(ConnectionPtr conn, NetMessage& req)
{
	int protoId = req.getProto();
	LogD << "Recv msg: " << req.getMessage() << "  protocol:" << req.getProto() << "  length:" << req.getLen() << LogEnd;
	if (protoId <= my::protocol::MYSQL_BEGIN || protoId >= my::protocol::MYSQL_END)
	{
		LogW << "Unknown protocol, protoId=" << protoId << LogEnd;
		return;
	}
	NetMessage rsp;
	funcHandlerMgr.runFuncHandler(req, rsp);
	rsp.serialize();
	conn->sendMessage(rsp);
}