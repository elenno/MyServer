/*
 * bookStoreMgr.cpp
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */

#include "../head/bookStoreMgr.h"
#include "../head/mongoMgr.h"
#include "funcHandler.h"
#include "log_system.h"
#include "helpFunctions.h"
#include "stringDef.h"

using namespace std;

my::BookStoreMgr::BookStoreMgr()
{
	printf("Init BookStoreMgr\n");
    m_BookMap.clear();
	m_BookNoMap.clear();
    m_dMoney = InitFund;
	loadFromDB();
	registFunc();
}

my::BookStoreMgr::~BookStoreMgr()
{
    m_BookMap.clear();
	m_BookNoMap.clear();
    m_dMoney = 0;
}

bool my::BookStoreMgr::sold(string bookNo, int num, double money)
{
    int bookId = getIdFromBookNo(bookNo);
    BookMap::iterator it = m_BookMap.find(bookId);
	if (it == m_BookMap.end())
	{
		return false;
	}
	Book::ptr book = it->second;
    if (num > book->getStock())
    {
    	return false;
    }
    if (money < book->getPrice() * num)
    {
    	return false;
    }
    if (!book->minusStock(num)) //¿Û³ýÊýÁ¿
    {
    	return false;
    }
	saveBookToDB(book->getBookId());
    m_dMoney += book->getPrice() * num; //¼ÓÇ®
	saveBookStore();
    printf("Book Sold: no=%s price=%.2f num=%d, Now store has %.2f dollar.\n", bookNo.c_str(), book->getPrice(), num, m_dMoney);
    return true;
}

bool my::BookStoreMgr::stock(string bookNo, int num, double cost)
{
	if (cost > m_dMoney)
	{
		printf("%s| not enough money to stock some books, bookNo=%s\n", __FUNCTION__, bookNo.c_str());
		return false;
	}
	int bookId = getIdFromBookNo(bookNo);
	BookMap::iterator it = m_BookMap.find(bookId);
	if (it == m_BookMap.end())
	{
		printf("Add Old Book Stock Failed! Can't find this book: no=%s\n", bookNo.c_str());
		return false; 
	}	
	Book::ptr& book = it->second;
	int tmp = book->getStock();
	book->addStock(num);
	printf("Add Old Book Stock Success: old=%d new=%d\n", tmp, book->getStock());
	saveBookToDB(book->getBookId());
	m_dMoney -= cost;
	saveBookStore();
	return true;
}

bool my::BookStoreMgr::setNewBook(string bookNo, double price, int num, double cost)
{
	if (m_dMoney < cost)
	{
		printf("Not Enough Money To Set New Book, money=%.2f cost=%.2f\n", m_dMoney, cost);
        return false;
	}

	int bookId = getIdFromBookNo(bookNo);
	if (-1 != bookId)
	{
		printf("Already Have This Book, id=%d no=%s\n", bookId, bookNo.c_str());
		return false;
	}
	m_dMoney -= cost;
	bookId = getBookCount() + 1;
	Book::ptr book = Book::create(bookId, bookNo.c_str(), num, price);
	m_BookMap.insert(make_pair<int, Book::ptr>(bookId, book));
	m_BookNoMap.insert(make_pair<string, int>(bookNo, bookId));
	saveBookToDB(bookId);
	saveBookStore();
	//findBook(bookId);
	printf("Set New Book: id=%d no=%s price=%.2f num=%d\n", bookId, bookNo.c_str(), price, num);
	return true;
}

bool my::BookStoreMgr::saveBookToDB(int bookId)
{
	BookMap::iterator it = m_BookMap.find(bookId);
	if (it == m_BookMap.end())
	{
		return false;
	}
	Book::ptr book = it->second;
	Json::Value json = book->toJson();
	Json::Value key;
	key[db::Book::bookId] = book->getBookId();
	string keyStr = key.toStyledString();
	string jsonStr = json.toStyledString();
	mongoMgr.saveJson(db::Book::colName, keyStr, jsonStr);
	return true;
}

int my::BookStoreMgr::getIdFromBookNo(string bookNo)
{
	BookNoMap::iterator it = m_BookNoMap.find(bookNo);
	if (it == m_BookNoMap.end())
	{
		printf("can't find bookNo:%s\n", bookNo.c_str());
		return -1;
	}
	return it->second;
}

int my::BookStoreMgr::getBookCount()
{
	return mongoMgr.db_count(db::Book::colName);
}

Json::Value my::BookStoreMgr::findBook(int bookId)
{
	Json::Value key;
	key[db::Book::bookId] = bookId;
	Json::Value ret = mongoMgr.findJson(db::Book::colName, key.toStyledString());
	return ret;
}

bool my::BookStoreMgr::loadFromDB()
{
	Json::Value key;
	key[db::DataBase::system] = db::DataBase::system;
	Json::Value books = mongoMgr.findJson(db::Book::colName);
	Json::Value bookStore = mongoMgr.findJson(db::BookStore::colName, key);
	if (!books.isArray())
	{
		return false;
	}
	for (int i = 0; i < books.size(); i++)
	{
		Json::Value& book = books[i];
		int bookId = book[db::Book::bookId].asInt();
		string bookNo = book[db::Book::bookNo].asString();
		int num = book[db::Book::stock].asInt();
		double price = book[db::Book::price].asDouble();
		Book::ptr bookPtr = Book::create(bookId, bookNo.c_str(), num, price);
		m_BookMap.insert(make_pair<int, Book::ptr>(bookId, bookPtr));
		m_BookNoMap.insert(make_pair<string, int>(bookNo, bookId));
	}
	if (bookStore.empty())
	{
		m_dMoney = InitFund;
	}
	else
	{
		m_dMoney = bookStore[db::BookStore::money].asDouble();
	}
	return true;
}

bool my::BookStoreMgr::saveBookStore()
{
	Json::Value key;
	Json::Value json;
	key[db::DataBase::system] = db::DataBase::system;
	json[db::DataBase::system] = db::DataBase::system;
	json[db::BookStore::money] = m_dMoney;
	string keyStr = key.toStyledString();
	string jsonStr = json.toStyledString();
	mongoMgr.saveJson(db::BookStore::colName, keyStr, jsonStr);
	return true;
}

void my::BookStoreMgr::show()
{
	printf("BookStore: money=%.2f\n", m_dMoney);
    BookMap::iterator it = m_BookMap.begin();
	for (; it != m_BookMap.end(); ++it)
	{
		Book::ptr p = it->second;
		printf("Book: id=%d, bookNo=%s, stock=%d, price=%.2f\n", p->getBookId(), p->getBookNo(), p->getStock(), p->getPrice());
	}
}

void my::BookStoreMgr::registFunc()
{
	//RegistFunc(api::SHOW_BOOK_LIST_REQ, api::SHOW_BOOK_LIST_RSP, BookStoreMgr::showReq);
	//RegistFunc(api::SET_NEW_BOOK_REQ, BookStoreMgr::setNewBookReq);
	//RegistFunc(api::SELL_BOOK_REQ, BookStoreMgr::soldReq);
	//RegistFunc(api::STOCK_OLD_BOOK_REQ, BookStoreMgr::stockReq);
}

bool my::BookStoreMgr::showReq(Json::Value& req, Json::Value& rsp)
{
	Json::Value data = Json::arrayValue;
	BookMap::iterator it = m_BookMap.begin();
	for (; it != m_BookMap.end(); ++it)
	{
		Json::Value book;
		Book::ptr p = it->second;
	    book[db::Book::bookId] = p->getBookId();
		book[db::Book::bookNo] = p->getBookNo();
		book[db::Book::price] = p->getPrice();
		book[db::Book::stock] = p->getStock();
		data.append(book);
	}
	rsp["data"] = data;

	LogD << my::HelpFunctions::tighten(rsp.toStyledString()) << LogEnd;
	return true;
}

bool my::BookStoreMgr::setNewBookReq(Json::Value& req, Json::Value& rsp)
{
	return true;
}

bool my::BookStoreMgr::stockReq(Json::Value& req, Json::Value& rsp)
{
	return true;
}

bool my::BookStoreMgr::soldReq(Json::Value& req, Json::Value& rsp)
{
	return true;
}