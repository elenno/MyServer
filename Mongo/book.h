/*
 * book.h
 *
 *  Created on: 2014-9-9
 *      Author: Administrator
 */

#ifndef __BOOK_H__
#define __BOOK_H__

#include "common.h"
#include<boost/shared_ptr.hpp>
//#include<nedmalloc/nedmalloc.h>
#include<json/json.h>
using namespace std;

namespace my
{
	class Book
	{
	public:
		Book();
		Book(int bookId, const char* bookNo, int stock, double price);
		~Book();

		int getBookId() const;
		double getPrice() const;
		int getStock() const;
		const char* getBookNo() const;
		void setPrice(double price);
		void setStock(int stock);
		void setBookNo(const char* bookNo);
		void setBookId(int bookId);
		void addStock(int num);
		bool minusStock(int num);
		Json::Value toJson();

		typedef boost::shared_ptr<Book> ptr;
		static ptr create();
		static ptr create(int bookId, const char* bookNo, int stock, double price);
		static void destroy(Book* p);

	private:
		int m_nBookId;
		char m_szBookNo[15];
		int m_nStock;
		double m_dPrice;
	};
}

#endif /* BOOK_H_ */
