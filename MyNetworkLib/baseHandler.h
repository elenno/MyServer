#ifndef __BASE_HANDLER_H__
#define __BASE_HANDLER_H__

#include "message.h"
#include "tcpConnection.h"

namespace my
{
	class TcpConnection;
	class BaseHandler
	{
	public:
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

		BaseHandler(){};
		virtual ~BaseHandler() = 0 {};

		virtual void onRecv(ConnectionPtr conn, NetMessage& req) = 0;

	private:

	};
}

#endif