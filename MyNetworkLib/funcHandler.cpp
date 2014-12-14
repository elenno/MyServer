#include "funcHandler.h"
#include "message.h"
#include "helpFunctions.h"

my::FuncHandler::FuncHandler()
{
	m_HandlerMap.clear();
	m_ProtoMap.clear();
}

my::FuncHandler::~FuncHandler()
{
    m_HandlerMap.clear();
	m_ProtoMap.clear();
}

void my::FuncHandler::registFuncHandler(int handlerId, int responseId, HandlerFunc handler)
{
	m_HandlerMap.insert(std::make_pair<int, HandlerFunc>(handlerId, handler));
    m_ProtoMap.insert(std::make_pair<int, int>(handlerId, responseId));
}

bool my::FuncHandler::runFuncHandler(NetMessage& reqMsg, NetMessage& rspMsg)
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
		HandlerFunc& handler = it->second;
		int responseId = it1->second;
		Json::Value rspJson;
		if (!handler(reqJson, rspJson))
		{
			return false;
		}
		std::string tmp = my::HelpFunctions::tighten(rspJson.toStyledString());
		rspMsg.setMessage(tmp);
		rspMsg.setProto(responseId);
	    return true;
	}
}