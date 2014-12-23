#include "accountMgr.h"
#include "protocol.h"
#include "helpFunctions.h"
#include "funcHandler.h"
#include "stringDef.h"
#include "log_system.h"

my::AccountMgr::AccountMgr()
{
	connectDB("127.0.0.1");
	registFunc();
	ensureIndex();
}

my::AccountMgr::~AccountMgr()
{

}

void my::AccountMgr::ensureIndex()
{
	m_MongoConn.ensureIndex(build_db_name(db::Account::colName), BSON(db::Account::playerId << 1), true);
}

void my::AccountMgr::registFunc()
{
	RegistFunc(my::protocol::PLAYER_REGIST_REQ, my::protocol::PLAYER_REGIST_RSP, AccountMgr::handlePlayerRegist);
	RegistFunc(my::protocol::PLAYER_LOGIN_REQ, my::protocol::PLAYER_LOGIN_RSP, AccountMgr::handlePlayerLogin);
}

bool my::AccountMgr::handlePlayerLogin(Json::Value& req, Json::Value& rsp, int playerId)
{
	if (!req["msg"].isArray() || req["msg"].size() < 2)
	{
		rsp["msg"][0u] = -1;
		return true;
	}
	std::string userName = req["msg"][0u].asString();
	std::string userPass = req["msg"][1u].asString();

	Json::Value key;
	key[db::Account::userName] = userName;
	Json::Value tmp = findJson(db::Account::colName, key.toStyledString());
	if (tmp.empty())
	{
		//找不到该账号
		rsp["msg"][0u] = 1;
	    return true;
	}
	if (tmp[db::Account::userPass].asString() != userPass)
	{
		//密码错误
		rsp["msg"][0u] = -1;
		return true;
	}

	rsp["msg"][0u] = 0;
	rsp["msg"][1u] = tmp[db::Account::playerId];
	return true;
}

bool my::AccountMgr::handlePlayerRegist(Json::Value& req, Json::Value& rsp, int playerId)
{
	if (!req["msg"].isArray() || req["msg"].size() < 2)
	{
		rsp["msg"][0u] = -1;
		return true;
	}

	std::string userName = req["msg"][0u].asString();
	std::string userPass = req["msg"][1u].asString();

	Json::Value key;
	key[db::Account::userName] = userName;
	Json::Value tmp = findJson(db::Account::colName, key.toStyledString());
	if (!tmp.empty())
	{
		//账号已被注册
		rsp["msg"][0u] = -1;
		return true;
	}
	int playerCount = db_count(db::Account::colName);
	time_t now;
	time(&now);
	Json::Value newPlayer;
	newPlayer[db::Account::playerId] = playerCount + 1;
	newPlayer[db::Account::userName] = userName;
	newPlayer[db::Account::userPass] = userPass;
	newPlayer[db::Account::registTime] = (unsigned int)(now);
	key = Json::nullValue;
	key[db::Account::playerId] = playerCount + 1;
	saveJson(db::Account::colName, key.toStyledString(), newPlayer.toStyledString());

	rsp["msg"][0u] = 0;
	rsp["msg"][1u] = playerCount + 1;
	return true;
}

bool my::AccountMgr::connectDB(const char* ip)
{
	string errorMsg;
	bool res = m_MongoConn.connect(ip, errorMsg);
	if (!res)
	{
		printf("Connect MongoDB Failed: %s", errorMsg.c_str());
	}
	return res;
}

bool my::AccountMgr::saveJson(const string& col_name, string& query, string& obj)
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

int my::AccountMgr::db_count(const string& col_name, string query  /*= "" */)
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

Json::Value my::AccountMgr::findJson(const string& col_name, string& query)
{
	string queryStr = my::HelpFunctions::tighten(query);
	Json::Value ret = Json::Value::null;
	Json::Reader reader;
	string colName = build_db_name(col_name);
	try{
		mongo::BSONObj obj = m_MongoConn.findOne(colName, queryStr);
		if (obj.isEmpty())
		{
			Json::Value::null;
		}
		if (!reader.parse(obj.jsonString(), ret))
		{
			return Json::Value::null;
		}
		ret.removeMember("_id");
	}catch(mongo::DBException& e)
	{
		printf("%s| caught exception: %s\n", __FUNCTION__, e.what());
		return Json::Value::null;
	}	
	return ret;
}

string my::AccountMgr::build_db_name(const string& str)
{
	string db_name = my::db::DataBase::dbName + "." + str;
	return db_name;
}