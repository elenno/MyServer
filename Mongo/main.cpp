/*
* main.cpp
*
*  Created on: 2014-9-5
*      Author: Administrator
*/

#include <iostream>
#include "client/dbclient.h"
#include "bson/bsonobj.h"
#include "../head/bookStoreMgr.h"
#include <boost/asio.hpp>
#include <string>
#include<string.h>
#include "tcpServer.h"
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include "file_system.h"
#include "log_system.h"
#include "../head/mongoMgr.h"
#include "../head/gameServer.h"
#pragma comment(lib,"ws2_32.lib")
using namespace mongo;
using namespace std;

char com[10];
string bookNo;
double price;
int num;
double cost;
double money;

int main()
{	
//	Json::Value json;
//	json["data"] = 1;
//	string jsonStr = json.toStyledString();
//	NetMessage msg(jsonStr);
//	printf("%s\n",msg.m_szBuff);

//	Json::Value val = fileSys.loadJsonFileEval(config::log_config);
//	printf("%s\n", val.toStyledString().c_str());
//	LogD << "test123123" << LogEnd;

	try
	{
		bool flag = false;
		do		
		{
			if (!gameSvr.init())
			{
				break;
			}	
			gameSvr.run();

		    bool res = mongoMgr.connectDB("127.0.0.1");
			if (!res)
			{
				LogD << "can't connect to Mongo" << LogEnd;
				break;
			}
			bookStoreMgr;

		    flag = true;
		}while(0);

		if (!flag)
		{
			LogE << "Server Init Error! Exit!" << LogEnd;
		}
		
		while(true)
		{
			printf("please enter a command: |quit|new|stock|sold|show| \n");
			scanf("%s", com);
			if (0 == strcmp("quit", com))
			{
				printf("bye bye!\n");
				break;
			}
			else if (0 == strcmp("new", com))
			{
				printf("please input 4 params: bookNo, price, num, cost\n");
				cin >> bookNo >> price >> num >> cost;
				bookStoreMgr.setNewBook(bookNo, price, num, cost);
			}
			else if (0 == strcmp("stock", com))
			{
				printf("please input 3 params: bookNo, num, cost\n");
				cin >> bookNo >> num >> money;
				bookStoreMgr.stock(bookNo, num, money);
			}
			else if (0 == strcmp("sold", com))
			{
				printf("please input 3 params: bookNo, num, money\n");
				cin >> bookNo >> num >> money;
				bookStoreMgr.sold(bookNo, num, money);
			}
			else if (0 == strcmp("show", com))
			{
				bookStoreMgr.show();
			}
			else
			{
				printf("command error! usage: |quit|new|stock|sold|show| \n");
			}
		}
	}
	catch (std::exception& e)
	{
		LogE << e.what() << LogEnd;
	}
	return 0;
}



