/*
 * book.cpp
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */
#include "book.h"
#include <string>
#include "stringDef.h"

using namespace std;

my::Book::Book()
{
	m_nBookId = 0;
    memset(m_szBookNo, 0, sizeof(m_szBookNo));
    m_nStock = 0;
    m_dPrice = 0;
}

my::Book::Book(int bookId, const char* bookNo, int stock, double price)
{
	m_nBookId = bookId;
    strncpy(m_szBookNo, bookNo, sizeof(m_szBookNo));
	m_szBookNo[sizeof(m_szBookNo) - 1] = '\0';
    m_nStock = stock;
    m_dPrice = price;
}

my::Book::~Book()
{
	m_nBookId = 0;
	memset(m_szBookNo, 0, sizeof(m_szBookNo));
	m_nStock = 0;
	m_dPrice = 0;
}

int my::Book::getBookId() const
{
	return m_nBookId;
}

double my::Book::getPrice() const
{
    return m_dPrice;
}

int my::Book::getStock() const
{
    return m_nStock;
}

const char* my::Book::getBookNo() const
{
    return m_szBookNo;
}

void my::Book::setPrice(double price)
{
    m_dPrice = price;
}

void my::Book::setStock(int stock)
{
    m_nStock = stock;
}

void my::Book::setBookNo(const char* bookNo)
{
    strncmp(m_szBookNo, bookNo, sizeof(m_szBookNo));
}

void my::Book::addStock(int num)
{
	if (num <= 0)
	{
		return;
	}
	m_nStock += num;
}

bool my::Book::minusStock(int num)
{
	if (num <= 0)
	{
		return false;
	}
	if (num > m_nStock)
	{
		return false;
	}
	m_nStock -= num;
	return true;
}

my::Book::ptr my::Book::create()
{
	void* m = nedalloc::nedmalloc(sizeof(Book));
	return ptr(new(m) Book(),destroy);
}

my::Book::ptr my::Book::create(int bookId, const char* bookNo, int stock, double price)
{
	void* m = nedalloc::nedmalloc(sizeof(Book));
	return ptr(new(m) Book(bookId, bookNo, stock, price),destroy);
}

void my::Book::destroy(Book* p)
{
	nedalloc::nedfree(p);
}

Json::Value my::Book::toJson()
{
	Json::Value ret;
	ret[db::Book::bookId] = m_nBookId;
	ret[db::Book::bookNo] = m_szBookNo;
	ret[db::Book::price] = m_dPrice;
	ret[db::Book::stock] = m_nStock;
	return ret;
}