#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__

#include "baseHandler.h"
#include "tcpConnection.h"

namespace my
{
	class TcpConnection;
	class MessageHandler : public BaseHandler
	{
	public:
		typedef boost::shared_ptr<MessageHandler> ptr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

		MessageHandler();
		virtual ~MessageHandler();

		virtual void onRecv(ConnectionPtr conn, NetMessage& req);

	private:

	};
}

#endif