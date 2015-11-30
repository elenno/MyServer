#include "accountServer.h"
#include "accountMgr.h"
#include "log_system.h"
int main()
{
	logSys.setServerName("account");
	accountMgr;
	accountSvr.init();
	accountSvr.run();
	while(1)
	{

	}
	return 0;
}