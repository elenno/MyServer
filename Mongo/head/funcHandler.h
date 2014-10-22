#ifndef __FUNC_HANDLER_H__
#define __FUNC_HANDLER_H__

#include <boost/function.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/bind.hpp>
#include "../common/common.h"
#include "json/json.h"
#include "../head/message.h"

#define funcHandlerMgr boost::detail::thread::singleton<FuncHandler>::instance()

#define RegistFunc(REQUEST_PROTOCOL, RESPONSE_PROTOCOL, FUNCTION) \
{\
	funcHandlerMgr.registFuncHandler(REQUEST_PROTOCOL, RESPONSE_PROTOCOL, boost::bind(&FUNCTION,this,_1,_2));\
}

class FuncHandler
{
public:
	typedef boost::function<bool (Json::Value&, Json::Value&)> Handler;
	typedef map<int, Handler> HandlerMap;
	typedef map<int, int> ProtoMap;

public:
	FuncHandler();
	~FuncHandler();

	void registFuncHandler(int handlerId, int responseId, Handler handler);
	bool runFuncHandler(NetMessage& reqMsg, NetMessage& rspMsg);

private:
	HandlerMap m_HandlerMap;
	ProtoMap m_ProtoMap;
};

#endif