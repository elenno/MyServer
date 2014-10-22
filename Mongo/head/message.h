#ifndef __NET__MESSAGE_H__
#define __NET__MESSAGE_H__

#include "json/json.h"
#include "../common/common.h"

class NetMessage
{
public:
	NetMessage(string& json_str, int proto);
	NetMessage();
	~NetMessage();

    string getMessage();
	void setMessage(const string& msg);
    int getLen();
	void setProto(int protoId);
	void serialize();
	bool deserialize(const char* buff, int size);
	const char* getStream();
    
private:
	int m_nLen;
	int m_nProto;
	string m_szMessage;
	char m_szStream[8192];
};

#endif