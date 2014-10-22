#include "../head/funcHandler.h"
#include "../head/bookStoreMgr.h"
#include "../head/message.h"
#include "../head/helpFunctions.h"

FuncHandler::FuncHandler()
{
	m_HandlerMap.clear();
	m_ProtoMap.clear();
}

FuncHandler::~FuncHandler()
{
    m_HandlerMap.clear();
	m_ProtoMap.clear();
}

void FuncHandler::registFuncHandler(int handlerId, int responseId, Handler handler)
{
	m_HandlerMap.insert(std::make_pair<int, Handler>(handlerId, handler));
    m_ProtoMap.insert(std::make_pair<int, int>(handlerId, responseId));
}

bool FuncHandler::runFuncHandler(NetMessage& reqMsg, NetMessage& rspMsg)
{
	Json::Value reqJson;
	Json::Reader reader;
	if (!reader.parse(reqMsg.getMessage(), reqJson))
	{
	    return false;
	}
	int handlerId = reqJson["msg"].asInt();
	HandlerMap::iterator it = m_HandlerMap.find(handlerId);
	ProtoMap::iterator it1 = m_ProtoMap.find(handlerId);
	if (it == m_HandlerMap.end() || it1 == m_ProtoMap.end())
	{
		printf("Can't find funcHandler, handlerId=%d", handlerId);
		return false;
	}
	else
	{
		Handler& handler = it->second;
		int responseId = it1->second;
		Json::Value rspJson;
		if (!handler(reqJson, rspJson))
		{
			return false;
		}
		string tmp = helpFunc.tighten(rspJson.toStyledString());
		rspMsg.setMessage(tmp);
		rspMsg.setProto(responseId);
	    return true;
	}
}