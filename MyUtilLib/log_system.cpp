#include "log_system.h"
#if defined(_WIN32)
#include "windows.h"
#endif
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "helpFunctions.h"

util::LogSystem::LogSystem()
{
    m_QueueThreadPtr = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&LogSystem::runQueue, this)));
	m_QueueThreadPtr->detach();
}

util::LogSystem::~LogSystem()
{

}

void util::LogSystem::setColor(int color)
{
#if defined(_WIN32)
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color );
#endif
}

std::stringstream& util::LogSystem::logProcess(std::stringstream& s, const char* fileName, unsigned int lineNum, const char* funcName, tm& date)
{
	s << "|" << addzero(date.tm_hour) << date.tm_hour << ":" << addzero(date.tm_min) << date.tm_min << ":" << addzero(date.tm_sec) << date.tm_sec << "|" 
	<< fileName << ":" << lineNum << "(" << funcName << ")|";
	return s;
}

void util::LogSystem::outputLogToFile(std::string& dir, std::string& content, tm& date)
{
	std::stringstream ss;
	std::fstream fs;
	ss << dir << (1900 + date.tm_year) << addzero(date.tm_mon + 1) << (date.tm_mon + 1) << addzero(date.tm_mday) << date.tm_mday << addzero(date.tm_hour) << date.tm_hour << ".log";
	fs.open(ss.str(), std::ios::out | std::ios::app);
	fs << content << std::endl;
	fs.close();
}

void util::LogSystem::endline(std::stringstream& s, int color, std::string dir, tm& date)
{
	std::string strtmp = s.str();
	pushQueue(date, strtmp, dir);
	setColor(color);
	std::cout << s.str() << std::endl;
	setColor(LogSystem::WHITE);
}

void util::LogSystem::pushQueue(tm& date, std::string& content, std::string& dir)
{
	LogData tmp;
	tmp.date = date;
	tmp.content = content;
	tmp.dir = dir;
    boost::recursive_mutex::scoped_lock lock(m_LogMutex);
	m_LogQueue.push(tmp);
}

void util::LogSystem::runQueue()
{
    while(1)
	{
		HelpFunctions::threadSleepSecond(1);
		//boost::recursive_mutex::scoped_lock lock(m_LogMutex);
		while(!m_LogQueue.empty())
		{
			LogData& data = m_LogQueue.front();
			outputLogToFile(data.dir, data.content, data.date);
			m_LogQueue.pop();
		}		
	}
}