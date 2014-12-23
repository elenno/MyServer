#ifndef __ACCOUNT_MGR_H__
#define __ACCOUNT_MGR_H__

#include <json/json.h>
#include <client/dbclient.h>
#include <boost/thread/detail/singleton.hpp>

#define accountMgr boost::detail::thread::singleton<my::AccountMgr>::instance()

namespace my
{
	class AccountMgr
	{
	public:
		AccountMgr();
		~AccountMgr();

		bool handlePlayerRegist(Json::Value& req, Json::Value& rsp, int playerId);
		bool handlePlayerLogin(Json::Value& req, Json::Value& rsp, int playerId);

		//db
		bool connectDB(const char* ip);
		bool saveJson(const string& col_name, string& query, string& obj);
		int db_count(const string& col_name, string query = "");
		Json::Value findJson(const string& col_name, string& query);
		std::string build_db_name(const std::string& colName);

	private:
		void registFunc();
		void ensureIndex();

	private:
		mongo::DBClientConnection m_MongoConn;
	};
}


#endif