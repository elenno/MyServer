#ifndef __NET__MESSAGE_H__
#define __NET__MESSAGE_H__

#include "json/json.h"
#include <string>

namespace my
{
	class NetMessage
	{
	public:
		NetMessage(std::string& json_str, int proto);
		NetMessage();
		~NetMessage();

		std::string getMessage();
		void setMessage(const std::string& msg);
		int getLen();
		void setProto(int protoId);
		int getProto();
		void serialize();
		bool deserialize(const char* buff, int size);
		const char* getStream();

	private:
		int m_nLen;
		int m_nProto;
		std::string m_szMessage;
		char m_szStream[8192];
	};
}

#endif