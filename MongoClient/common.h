#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <map>
#include <iostream>

using namespace std;

#define Int64 __int64

#define Between(X, L, H) (((X) >= (L) && (X) <= (H)))

namespace db
{
	namespace DataBase
	{
		static const string dbName = "bs";
	    static const string system = "sys"; //系统数据统一取sys
	}

	namespace BookStore
	{
		static const string colName = "store";
		static const string money = "mo";
	};

	namespace Book
	{
		static const string colName = "book"; 
		static const string bookId = "bid";
		static const string bookNo = "bno";
		static const string stock = "sto";
		static const string price = "pri";
	};
}

namespace api
{
	static const int SHOW_BOOK_LIST_REQ = 10001;
	static const int SHOW_BOOK_LIST_RSP = 10002;
	static const int SET_NEW_BOOK_REQ = 10003;
	static const int SET_NEW_BOOK_RSP = 10004;
	static const int SELL_BOOK_REQ = 10005;
	static const int SELL_BOOK_RSP = 10006;
	static const int STOCK_OLD_BOOK_REQ = 10007;
	static const int STOCK_OLD_BOOK_RSP = 10008;
}

#endif