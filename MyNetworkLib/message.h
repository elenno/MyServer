#ifndef __NET__MESSAGE_H__
#define __NET__MESSAGE_H__

#include "json/json.h"
#include <string>

#define MSG_HEAD 16
#define MSG_MAXIMUM 8192
namespace my
{
	class NetMessage
	{
	public:
		NetMessage(std::string& json_str, int proto, int playerId, int netId);
		NetMessage();
		~NetMessage();

		std::string getMessage();
		void setMessage(const std::string& msg);
		int getLen();
		void setProto(int protoId);
		int getProto();
		void setPlayerId(int playerId);
		int getPlayerId();
		void setNetId(int netId);
		int getNetId();
		bool serialize();
		int deserialize(const char* buff, int size);
		const char* getStream();

	private:
		int m_nPlayerId;
		int m_nLen;
		int m_nProto;
		int m_nNetId;
		std::string m_szMessage;
		char* m_szStream;
		int m_nContentLength; //jsonµÄ³¤¶È
	};
}

#endif