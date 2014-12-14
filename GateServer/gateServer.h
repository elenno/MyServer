#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__

#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <string>
#include "tcpServer.h"
#include "tcpConnection.h"

using namespace boost::asio;

namespace my
{
	class GateServer : public TcpServer
	{
	public:
		typedef boost::shared_ptr<TcpConnection> ConnectionPtr;
		typedef std::map<int, ConnectionPtr> ConnectionMap;
		typedef boost::shared_ptr<GateServer> ptr;

	public:
		GateServer();
		~GateServer();

		void connectToGameSvr(std::string ipaddr, int port);
		void init();

		void handle_accept(ConnectionPtr conn, boost::system::error_code err); // 重写handle_accept
		void handle_connect(ConnectionPtr conn, boost::system::error_code err);

	private:
		void connect(std::string ipaddr, std::string port, ConnectionPtr conn);

	private:

		ConnectionMap m_ConnMap;
		int m_nConnCount;
		ConnectionPtr m_pGameConn; // 用于连接到game


	};
}

#endif