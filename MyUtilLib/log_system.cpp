#include "log_system.h"
#include "windows.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

my::LogSystem::LogSystem()
{

}

my::LogSystem::~LogSystem()
{

}

void my::LogSystem::setColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color );
}

std::stringstream& my::LogSystem::logDebug(const char* fileName, const char* funcName)
{
	setColor(LogSystem::GREEN);
	preLog(fileName, funcName);
	m_szLogPath = log_debug;
	return m_SStream;
}

std::stringstream& my::LogSystem::logWarn(const char* fileName, const char* funcName)
{
	setColor(LogSystem::YELLOW);
	preLog(fileName, funcName);
	m_szLogPath = log_warn;
	return m_SStream;
}

void my::LogSystem::outputLogToFile(std::string dir, std::string content)
{
	time_t now = time(NULL);
	tm* pnow = localtime(&now);
	std::stringstream ss;
	std::fstream fs;
	ss << dir << (1900 + pnow->tm_year) << zero(pnow->tm_mon + 1) << (pnow->tm_mon + 1) << zero(pnow->tm_mday) << pnow->tm_mday << zero(pnow->tm_hour) << pnow->tm_hour << ".log";
	fs.open(ss.str(), std::ios::out | std::ios::app);
	fs << content << std::endl;
	std::cout << content << std::endl;
	fs.close();

}

void my::LogSystem::preLog(const char* fileName, const char* funcName)
{
	m_Now = time(NULL);
	m_pNow = localtime(&m_Now);
	m_SStream << "|" <<zero(m_pNow->tm_hour) << m_pNow->tm_hour << ":" << zero(m_pNow->tm_min) << m_pNow->tm_min << ":" << zero(m_pNow->tm_sec) << m_pNow->tm_sec << "|" 
		<< fileName << ":" << funcName << "| ";
}

void my::LogSystem::endline()
{
	//boost::thread logThread(boost::bind(&LogSystem::outputLogToFile, this, m_szLogPath, m_SStream.str()));
	//logThread.detach();
	outputLogToFile(m_szLogPath, m_SStream.str());
	m_SStream.str("");
	setColor(LogSystem::GRAY);
}