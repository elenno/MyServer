#ifndef __STRING_DEFINE_H__
#define __STRING_DEFINE_H__

#include<string>

/************************************************************************/
/* json config                                                          */
/************************************************************************/

namespace my
{
	namespace jsonconf
	{
		static const std::string gateConf = "../config/static/gateSvr/gateSvr.json";
		static const std::string gameConf = "../config/static/gameSvr/gameSvr.json";
		static const std::string accountConf = "../config/static/accountSvr/accountSvr.json";
		static const std::string log_config = "config/log_config.json";
	}

	namespace db
	{
		namespace DataBase
		{
			static const std::string dbName = "bs";
			static const std::string system = "sys"; //系统数据统一取sys
		}

		namespace BookStore
		{
			static const std::string colName = "store";
			static const std::string money = "mo";
		}

		namespace Book
		{
			static const std::string colName = "book"; 
			static const std::string bookId = "bid";
			static const std::string bookNo = "bno";
			static const std::string stock = "sto";
			static const std::string price = "pri";
		}

		namespace Player
		{
			static const std::string colName = "player";
			static const std::string playerId = "pi";
			static const std::string nickName = "nn";
			static const std::string level = "lv";
			static const std::string vip = "vip";
		}

		namespace Account
		{
			static const std::string colName = "account";
			static const std::string playerId = "pi";
			static const std::string userName = "un";
			static const std::string userPass = "up";
			static const std::string registTime = "rt";
		}
	}

	namespace logType
	{
		static const std::string log_debug = "log/debug/";
		static const std::string log_warn = "log/warn/";
	}
}

#endif