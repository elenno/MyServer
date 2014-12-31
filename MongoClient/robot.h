#ifndef __ROBOT_H__
#define __ROBOT_H__

#include <json/json.h>
#include "message.h"

namespace my
{
	class Robot
	{
	public:
		Robot(std::string prefix, int id);
		~Robot();

		int getStep();
		void setStep(int step);
		Json::Value& getPlayerInfo();
		Json::Value& getAccountInfo();

		void doAction(Json::Value& msg, int& step, int& playerId);
		void handleMsg(NetMessage& msg);
		void reset();

	public:
		void handleLoginReq(Json::Value& msg);
		void handleLoginRsp(Json::Value& msg);
		void handleRegistReq(Json::Value& msg);
		void handleRegistRsp(Json::Value& msg);
		void handleEnterGameReq(Json::Value& msg);
		void handleEnterGameRsp(Json::Value& msg);
		void handleCreateRoleReq(Json::Value& msg);
		void handleCreateRoleRsp(Json::Value& msg);
		void handleHeartBeatReq(Json::Value& msg);
		void handleHeartBeatRsp(Json::Value& msg);

	public:
		void init(std::string prefix, int id);

	private:
		Json::Value m_PlayerInfo;
		Json::Value m_AccountInfo;
		int m_nStep;
	};
}



#endif