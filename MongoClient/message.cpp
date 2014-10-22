#include "message.h"
#include "helpFunctions.h"

NetMessage::NetMessage(string& json_str, int proto)
{ 
	m_szMessage = json_str;
	m_nProto = proto;
	m_nLen = 0;
	memset(m_szStream, 0, sizeof(m_szStream));
}

NetMessage::NetMessage()
{
	m_nProto = 0;
	m_nLen = 0;
	memset(m_szStream, 0, sizeof(m_szStream));
}

NetMessage::~NetMessage()
{

}

string NetMessage::getMessage()
{
    return m_szMessage;
}

int NetMessage::getLen()
{
	return m_nLen;
}

const char* NetMessage::getStream()
{
	return m_szStream;
}

void NetMessage::setProto(int protoId)
{
	m_nProto = protoId;
}

void NetMessage::setMessage(const string& msg)
{
	m_szMessage = msg;
}

void NetMessage::serialize()
{
	memcpy(m_szStream + m_nLen, &m_nProto, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);

	memcpy(m_szStream + m_nLen, m_szMessage.c_str(), m_szMessage.length());
	m_nLen += m_szMessage.length();
}

bool NetMessage::deserialize(const char* buff, int size)
{
	memcpy(&m_nProto, buff + m_nLen, sizeof(m_nProto));
	m_nLen += sizeof(m_nProto);
	size -= sizeof(m_nProto);

	m_szMessage.assign(buff + m_nLen, size);
	return true;
}