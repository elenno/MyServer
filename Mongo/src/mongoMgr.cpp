/*
 * mongoMgr.cpp
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */

#include "../head/mongoMgr.h"
#include "helpFunctions.h"


my::MongoMgr::MongoMgr()
{
	//connectDB("127.0.0.1");
}

my::MongoMgr::~MongoMgr()
{

}

bool my::MongoMgr::connectDB(const char* ip)
{
	string errorMsg;
	bool res = m_MongoConn.connect(ip, errorMsg);
	if (!res)
	{
		printf("Connect MongoDB Failed: %s", errorMsg.c_str());
	}
	return res;
}

bool my::MongoMgr::saveJson(const string& col_name, string& query, string& obj)
{
	string colName;
	try
	{
		string queryStr = my::HelpFunctions::tighten(query);
		string objStr = my::HelpFunctions::tighten(obj);
		colName = build_db_name(col_name);
		m_MongoConn.update(colName, mongo::Query(queryStr), mongo::fromjson(objStr), true);//upsert为true则找不到则插入
	}
	catch(mongo::DBException& e)
	{
		printf("caught exception: %s\n", e.what());
		printf("error dbname:%s, query:%s, obj:%s", colName.c_str(), query.c_str(), obj.c_str());
		return false;
	}
	return true;
}

bool my::MongoMgr::saveJson2(const string& col_name, Json::Value& query, Json::Value& obj)
{
	string queryStr = my::HelpFunctions::tighten(query.toStyledString());
	string objStr = my::HelpFunctions::tighten(obj.toStyledString());
	return saveJson(col_name, queryStr, objStr);
}

int my::MongoMgr::db_count(const string& col_name, string query  /*= "" */)
{
	string colName = build_db_name(col_name);
	printf("db_count: dbname:%s", col_name.c_str());
	mongo::BSONObj bsonQuery;
	if (query != "")
	{
		bsonQuery = mongo::fromjson(query);
	}
	try{
		return m_MongoConn.count(colName);
	}catch(mongo::DBException& e)
	{
		printf("caught exception: %s\n", e.what());
		printf("for dbname:%s , query:%s\n", colName.c_str(), query.c_str());
		return 0;
	}
}

Json::Value my::MongoMgr::findJson(const string& col_name, string& query)
{
	string queryStr = my::HelpFunctions::tighten(query);
	Json::Value ret;
	Json::Reader reader;
	string colName = build_db_name(col_name);
	try{
		mongo::BSONObj obj = m_MongoConn.findOne(colName, queryStr);
		reader.parse(obj.jsonString(), ret);
		//printf("%s : %s  %s\n", __FUNCTION__, obj.jsonString().c_str(), queryStr.c_str());
	}catch(mongo::DBException& e)
	{
		printf("%s| caught exception: %s\n", __FUNCTION__, e.what());
	}	
	return ret;
}

Json::Value my::MongoMgr::findJson(const string& col_name, Json::Value& queryJson)
{
	string query = my::HelpFunctions::tighten(queryJson.toStyledString());
	return findJson(col_name, query);
}

Json::Value my::MongoMgr::findJson(const string& col_name)
{
	Json::Value ret = Json::arrayValue;
	string colName = build_db_name(col_name);
	std::auto_ptr< mongo::DBClientCursor > cursor = m_MongoConn.query(colName);
	try{
		while(cursor->more())
		{
			mongo::BSONObj obj = cursor->next();
			Json::Reader reader;
			string tmp = obj.jsonString();
			Json::Value p;
			if (reader.parse(tmp, p))
			{
				ret.append(p);
			}
		}
	}catch(mongo::DBException& e)
	{
		printf("%s| caught exception: %s\n", __FUNCTION__, e.what());
	}
	return ret;
}

string my::MongoMgr::build_db_name(const string& str)
{
	string db_name = db::DataBase::dbName + "." + str;
	return db_name;
}

