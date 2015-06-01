#include "message.h"
#include <string.h>

my::NetMessage::NetMessage(std::string& json_str, int proto, int playerId, int netId)
{ 
	m_szMessage = json_str;
	m_nProto = proto;
	m_nPlayerId = playerId;
	m_nNetId = netId;
	m_nLen = 0;
	m_nContentLength = json_str.length();
	m_szStream = NULL;
}

my::NetMessage::NetMessage()
{
	m_nProto = 0;
	m_nLen = 0;
	m_nPlayerId = 0;
	m_nNetId = 0;
	m_nContentLength = 0;
	m_szStream = NULL;
}

my::NetMessage::~NetMessage()
{
	delete[] m_szStream;
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
	m_nContentLength = msg.length();
}

std::string my::NetMessage::getMessage()
{
	return m_szMessage;
}

bool my::NetMessage::serialize()
{
	if (MSG_HEAD + m_szMessage.length() >= MSG_MAXIMUM)
	{
		return false;
	}
	m_szStream = new char[MSG_HEAD + m_szMessage.length()];

	m_nLen = 0;

	memcpy(m_szStream + m_nLen, &m_nProto, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);

	memcpy(m_szStream + m_nLen, &m_nPlayerId, sizeof(m_nPlayerId));
	m_nLen += sizeof(m_nPlayerId);

	memcpy(m_szStream + m_nLen, &m_nNetId, sizeof(m_nNetId));
	m_nLen += sizeof(m_nNetId);

	memcpy(m_szStream + m_nLen, &m_nContentLength, sizeof(m_nContentLength));
	m_nLen += sizeof(m_nContentLength);

	memcpy(m_szStream + m_nLen, m_szMessage.data(), m_szMessage.length());
	m_nLen += m_szMessage.length();

	return true;
}

int my::NetMessage::deserialize(const char* buff, int size)
{
	if (size >= MSG_MAXIMUM || size < MSG_HEAD)
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

	memcpy(&m_nContentLength, buff + m_nLen, sizeof(m_nContentLength));
	m_nLen += sizeof(m_nContentLength);
	size -= sizeof(m_nContentLength);

	if (size >= m_nContentLength && m_nContentLength >= 0)
	{
		m_szMessage.assign(buff + m_nLen, m_nContentLength);
		m_nLen += m_nContentLength;
		return m_nLen;
	}
	else
	{
		return 0;
	}
}