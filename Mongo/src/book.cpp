/*
 * book.cpp
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */
#include "../head/book.h"
#include <string>

using namespace std;

Book::Book()
{
	m_nBookId = 0;
    memset(m_szBookNo, 0, sizeof(m_szBookNo));
    m_nStock = 0;
    m_dPrice = 0;
}

Book::Book(int bookId, const char* bookNo, int stock, double price)
{
	m_nBookId = bookId;
    strncpy(m_szBookNo, bookNo, sizeof(m_szBookNo));
	m_szBookNo[sizeof(m_szBookNo) - 1] = '\0';
    m_nStock = stock;
    m_dPrice = price;
}

Book::~Book()
{
	m_nBookId = 0;
	memset(m_szBookNo, 0, sizeof(m_szBookNo));
	m_nStock = 0;
	m_dPrice = 0;
}

int Book::getBookId() const
{
	return m_nBookId;
}

double Book::getPrice() const
{
    return m_dPrice;
}

int Book::getStock() const
{
    return m_nStock;
}

const char* Book::getBookNo() const
{
    return m_szBookNo;
}

void Book::setPrice(double price)
{
    m_dPrice = price;
}

void Book::setStock(int stock)
{
    m_nStock = stock;
}

void Book::setBookNo(const char* bookNo)
{
    strncmp(m_szBookNo, bookNo, sizeof(m_szBookNo));
}

void Book::addStock(int num)
{
	if (num <= 0)
	{
		return;
	}
	m_nStock += num;
}

bool Book::minusStock(int num)
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

Book::ptr Book::create()
{
	void* m = nedalloc::nedmalloc(sizeof(Book));
	return ptr(new(m) Book(),destroy);
}

Book::ptr Book::create(int bookId, const char* bookNo, int stock, double price)
{
	void* m = nedalloc::nedmalloc(sizeof(Book));
	return ptr(new(m) Book(bookId, bookNo, stock, price),destroy);
}

void Book::destroy(Book* p)
{
	nedalloc::nedfree(p);
}

Json::Value Book::toJson()
{
	Json::Value ret;
	ret[db::Book::bookId] = m_nBookId;
	ret[db::Book::bookNo] = m_szBookNo;
	ret[db::Book::price] = m_dPrice;
	ret[db::Book::stock] = m_nStock;
	return ret;
}