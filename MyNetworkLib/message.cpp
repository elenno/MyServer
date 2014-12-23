#include "message.h"

my::NetMessage::NetMessage(std::string& json_str, int proto, int playerId)
{ 
	m_szMessage = json_str;
	m_nProto = proto;
	m_nPlayerId = playerId;
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

std::string my::NetMessage::getMessage()
{
    return m_szMessage;
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

void my::NetMessage::serialize()
{
	m_nLen = 0;
	memcpy(m_szStream + m_nLen, &m_nProto, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);

	memcpy(m_szStream + m_nLen, &m_nPlayerId, sizeof(m_nPlayerId));
	m_nLen += sizeof(m_nPlayerId);

	memcpy(m_szStream + m_nLen, m_szMessage.c_str(), m_szMessage.length());
	m_nLen += m_szMessage.length();
}

bool my::NetMessage::deserialize(const char* buff, int size)
{
	m_nLen = 0;
	memcpy(&m_nProto, buff + m_nLen, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);
	size -= sizeof(m_nProto);

	memcpy(&m_nPlayerId, buff + m_nLen, sizeof(m_nPlayerId));
	m_nLen += sizeof(m_nPlayerId);
	size -= sizeof(m_nPlayerId);

	m_szMessage.assign(buff + m_nLen, size);
	return true;
}