#ifndef __ACCOUNT_HANDLER_H__
#define __ACCOUNT_HANDLER_H__

#include "baseHandler.h"
#include "tcpConnection.h"
#include "message.h"
#include <boost/shared_ptr.hpp>

namespace my
{
	class AccountHandler : public BaseHandler
	{
	public:
		typedef boost::shared_ptr<AccountHandler> ptr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

		AccountHandler();
		virtual ~AccountHandler();

		virtual void onRecv(ConnectionPtr conn, NetMessage& req);

	private:
	};
}

#endif