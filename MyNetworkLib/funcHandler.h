#ifndef __FUNC_HANDLER_H__
#define __FUNC_HANDLER_H__

#include <boost/function.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/bind.hpp>
#include "json/json.h"
#include "message.h"
#include "baseHandler.h"
#include <map>

#define funcHandlerMgr boost::detail::thread::singleton<my::FuncHandler>::instance()

#define RegistFunc(REQUEST_PROTOCOL, RESPONSE_PROTOCOL, FUNCTION) \
{\
	funcHandlerMgr.registFuncHandler(REQUEST_PROTOCOL, RESPONSE_PROTOCOL, boost::bind(&FUNCTION,this,_1,_2,_3));\
}

namespace my
{
	class FuncHandler
	{
	public:
		typedef boost::function<bool (Json::Value&, Json::Value&, int)> HandlerFunc;
		typedef std::map<int, HandlerFunc> HandlerMap;
		typedef std::map<int, int> ProtoMap;

	public:
		FuncHandler();
		~FuncHandler();

		void registFuncHandler(int handlerId, int responseId, HandlerFunc handler);
		bool runFuncHandler(NetMessage& reqMsg, NetMessage& rspMsg);

	private:
		HandlerMap m_HandlerMap;
		ProtoMap m_ProtoMap;
	};
}

#endif