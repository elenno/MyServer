/*
 * bookStoreMgr.h
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */

#ifndef __BOOK_STORE_MGR_H__
#define __BOOK_STORE_MGR_H__

#include <boost/thread/detail/singleton.hpp>
#include "book.h"
#include "nedmalloc/nedmalloc.h"
#include "common.h"
#include "json/json.h"

using namespace std;

#define InitFund 1000
#define bookStoreMgr boost::detail::thread::singleton<my::BookStoreMgr>::instance()

namespace my
{
	class BookStoreMgr
	{
	public:
		BookStoreMgr();
		~BookStoreMgr();

		//client api
		void registFunc();
		bool showReq(Json::Value& req, Json::Value& rsp);
		bool setNewBookReq(Json::Value& req, Json::Value& rsp);
		bool stockReq(Json::Value& req, Json::Value& rsp);
		bool soldReq(Json::Value& req, Json::Value& rsp);

		//data
		bool sold(string bookNo, int num, double money);
		bool stock(string bookNo, int num, double cost);
		bool setNewBook(string bookNo, double price, int num, double cost);
		void show();
		bool saveBookToDB(int bookId);
		bool saveBookStore();
		bool loadFromDB();

		int getIdFromBookNo(string bookNo);
		int getBookCount();
		Json::Value findBook(int bookId);

	private:

		typedef map<int, Book::ptr> BookMap;
		BookMap m_BookMap;
		typedef map<string, int> BookNoMap;
		BookNoMap m_BookNoMap;
		double m_dMoney;
	};
}

#endif /* BOOKSTOREMGR_H_ */
