/*
 * mongoMgr.h
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */

#ifndef __MONGO_MGR_H__
#define __MONGO_MGR_H__

#include <boost/thread/detail/singleton.hpp>
#include "../common/common.h"
#include <client/dbclient.h>
#include "json/json.h"

#define mongoMgr boost::detail::thread::singleton<my::MongoMgr>::instance()

namespace my
{
	class MongoMgr
	{
	public:
		MongoMgr();
		~MongoMgr();

		bool connectDB(const char* ip);
		bool saveJson(const string& col_name, string& query, string& obj);
		bool saveJson2(const string& col_name, Json::Value& query, Json::Value& obj);
		int db_count(const string& col_name, string query = "");
		Json::Value findJson(const string& col_name, string& query);
		Json::Value findJson(const string& col_name, Json::Value& queryJson);
		Json::Value findJson(const string& col_name);

	private:
		string build_db_name(const string& str);

		mongo::DBClientConnection m_MongoConn;
	};
}

#endif /* MONGOMGR_H_ */
