#include "tcpServer.h"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "gateServer.h"

int main()
{
	gateSvr.init();
	gateSvr.run();
	while(1)
	{

	}
	return 0;
}