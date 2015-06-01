#include "funcHandler.h"
#include "message.h"
#include "helpFunctions.h"
#include "log_system.h"
#include <json/json.h>

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

void my::FuncHandler::registFuncHandler(int requestId, int responseId, HandlerFunc handler)
{
	m_HandlerMap.insert(HandlerMap::value_type(requestId, handler));
    m_ProtoMap.insert(ProtoMap::value_type(requestId, responseId));
}

bool my::FuncHandler::runFuncHandler(NetMessage& reqMsg, NetMessage& rspMsg)
{
	Json::Value reqJson;
	Json::Reader reader;
	int playerId = reqMsg.getPlayerId();
	int netId = reqMsg.getNetId();
	std::string msgStr = reqMsg.getMessage();
	if (msgStr != "" && !reader.parse(msgStr, reqJson))
	{
	    return false;
	}
	int handlerId = reqMsg.getProto();
	HandlerMap::iterator it = m_HandlerMap.find(handlerId);
	ProtoMap::iterator it1 = m_ProtoMap.find(handlerId);
	if (it == m_HandlerMap.end() || it1 == m_ProtoMap.end())
	{
		LogW << "Can't find funcHandler, handlerId=" << handlerId << LogEnd;
		return false;
	}
	else
	{
		HandlerFunc& handler = it->second;
		int responseId = it1->second;
		Json::Value rspJson;
		if (!handler(reqJson, rspJson, playerId))
		{
			return false;
		}
		std::string tmp = util::HelpFunctions::tighten(rspJson.toStyledString());
		rspMsg.setMessage(tmp);
		rspMsg.setProto(responseId);
		rspMsg.setPlayerId(playerId);
		rspMsg.setNetId(netId);
	    return true;
	}
}