#ifndef __MYSQL_HANDLER_H__
#define __MYSQL_HANDLER_H__

#include "mysqlServer.h"

namespace my
{
	class MysqlHandler : public BaseHandler
	{
	public:
		typedef boost::shared_ptr<MysqlHandler> ptr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

		MysqlHandler();
		virtual ~MysqlHandler();
		virtual void onRecv(ConnectionPtr conn, NetMessage& req);

	private:
	};
}

#endif