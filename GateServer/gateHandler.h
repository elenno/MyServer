#ifndef __GATE_HANDLER_H__
#define __GATE_HANDLER_H__

#include "baseHandler.h"
#include "message.h"

namespace my
{
	class GateHandler : public BaseHandler
	{
	public:
		typedef boost::shared_ptr<GateHandler> ptr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

		GateHandler();
		virtual ~GateHandler();

		virtual void onRecv(ConnectionPtr conn, NetMessage& req);
		void handleServerMsg(ConnectionPtr conn, NetMessage& req);
		void handleClientMsg(ConnectionPtr conn, NetMessage& req);

	private:
	};
}

#endif

