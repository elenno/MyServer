#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__

#include <string>
#include <sstream>
#include <queue>
#include <boost/thread/detail/singleton.hpp>
#include <boost/thread.hpp>
#include "helpFunctions.h"

#define logSys boost::detail::thread::singleton<util::LogSystem>::instance()

#define addzero(NUM) ((NUM) < 10 ? "0" : "")

static const std::string log_debug = "log/debug/";
static const std::string log_warn = "log/warn/";   //要改为读入配置文件
static const std::string log_info = "log/info/";

#define LogEnd \
	std::endl;     \
	logSys.endline(log_ss, log_color, log_path, *log_pdate);\
	}while(0)

#define LogProcess \
	tm* log_pdate = NULL;\
    log_pdate = util::HelpFunctions::getCurrentTM(log_pdate);\
	std::stringstream log_ss;\
	logSys.logProcess(log_ss, __FILE__, __LINE__, __FUNCTION__, *log_pdate)

#define LogD \
	do{\
        int log_color = util::LogSystem::GREEN;\
        std::string log_path = log_debug;\
	    LogProcess

#define LogW \
	do{\
	    int log_color = util::LogSystem::YELLOW;\
	    std::string log_path = log_warn;\
	    LogProcess

#define LogI \
	do{\
	    int log_color = util::LogSystem::BLUE;\
	    std::string log_path = log_info;\
	    LogProcess

namespace util
{
	struct LogData
	{
		std::string dir;
		std::string content;
		tm date;
	};

	class LogSystem
	{
	public:
		LogSystem();
		~LogSystem();

		std::stringstream& logProcess(std::stringstream& s, const char* fileName, unsigned int lineNum, const char* funcName, tm& date);
		void endline(std::stringstream& s, int color, std::string dir, tm& date);
		
		enum Color {    
			DARKBLUE = 1, 
			DARKGREEN, 
			DARKTEAL, 
			DARKRED, 
			DARKPINK, 
			DARKYELLOW, 
			GRAY, 
			DARKGRAY, 
			BLUE, 
			GREEN, 
			TEAL, 
			RED, 
			PINK, 
			YELLOW, 
			WHITE 
		};

	private:
		void setColor(int color);
		void pushQueue(tm& data, std::string& content, std::string& dir);
		void runQueue();
		void outputLogToFile(std::string& dir, std::string& content, tm& date);

	private:
		boost::recursive_mutex m_LogMutex;
		std::queue<LogData> m_LogQueue; // todo  make a thread to write log into the file which in the queue 
		boost::shared_ptr<boost::thread> m_QueueThreadPtr;
	};
}


		
#endif