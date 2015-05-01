#ifndef __GAME_HANDLER_H__
#define __GAME_HANDLER_H__

#include "baseHandler.h"
#include "message.h"
#include "common.h"
#include "tcpConnection.h"

namespace my
{
	class GameHandler : public BaseHandler
	{
	public:
		typedef boost::shared_ptr<GameHandler> ptr;
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;

		GameHandler();
		virtual ~GameHandler();

		virtual void onRecv(ConnectionPtr conn, NetMessage& req);

	private:
	};
}

#endif