#include "message.h"
#include <string.h>

my::NetMessage::NetMessage(std::string& json_str, int proto, int playerId, int netId)
{ 
	m_szMessage = json_str;
	m_nProto = proto;
	m_nPlayerId = playerId;
	m_nNetId = netId;
	m_nLen = 0;
	memset(m_szStream, 0, sizeof(m_szStream));
}

my::NetMessage::NetMessage()
{
	m_nProto = 0;
	m_nLen = 0;
	m_nPlayerId = 0;
	memset(m_szStream, 0, sizeof(m_szStream));
}

my::NetMessage::~NetMessage()
{

}

int my::NetMessage::getNetId()
{
	return m_nNetId;
}

void my::NetMessage::setNetId(int netId)
{
	m_nNetId = netId;
}

int my::NetMessage::getLen()
{
	return m_nLen;
}

const char* my::NetMessage::getStream()
{
	return m_szStream;
}

void my::NetMessage::setProto(int protoId)
{
	m_nProto = protoId;
}

int my::NetMessage::getProto()
{
	return m_nProto;
}

void my::NetMessage::setPlayerId(int playerId)
{
	m_nPlayerId = playerId;
}

int my::NetMessage::getPlayerId()
{
	return m_nPlayerId;
}

void my::NetMessage::setMessage(const std::string& msg)
{
	m_szMessage = msg;
}

std::string my::NetMessage::getMessage()
{
	return m_szMessage;
}

void my::NetMessage::serialize()
{
	m_nLen = 0;
	if (m_nLen + sizeof(m_nProto) + sizeof(m_nPlayerId) + sizeof(m_nNetId) + m_szMessage.length() >= sizeof(m_szStream))
	{
		return;
	}

	memcpy(m_szStream + m_nLen, &m_nProto, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);

	memcpy(m_szStream + m_nLen, &m_nPlayerId, sizeof(m_nPlayerId));
	m_nLen += sizeof(m_nPlayerId);

	memcpy(m_szStream + m_nLen, &m_nNetId, sizeof(m_nNetId));
	m_nLen += sizeof(m_nNetId);

	memcpy(m_szStream + m_nLen, m_szMessage.data(), m_szMessage.length());
	m_nLen += m_szMessage.length();
}

bool my::NetMessage::deserialize(const char* buff, int size)
{
	if (size >= sizeof(m_szStream))
	{
		return false;
	}

	m_nLen = 0;
	memcpy(&m_nProto, buff + m_nLen, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);
	size -= sizeof(m_nProto);

	memcpy(&m_nPlayerId, buff + m_nLen, sizeof(m_nPlayerId));
	m_nLen += sizeof(m_nPlayerId);
	size -= sizeof(m_nPlayerId);

	memcpy(&m_nNetId, buff + m_nLen, sizeof(m_nNetId));
	m_nLen += sizeof(m_nNetId);
	size -= sizeof(m_nNetId);

	m_szMessage.assign(buff + m_nLen, size);
	m_nLen += size;
	return true;
}