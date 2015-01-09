#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__

#include <string>
#include <sstream>
#include <boost/thread/detail/singleton.hpp>

#define logSys boost::detail::thread::singleton<my::LogSystem>::instance()

#define zero(NUM) ((NUM) < 10 ? "0" : "")

#define LogEnd \
std::endl;     \
logSys.endline()

#define LogD logSys.logDebug(__FILE__, __FUNCTION__)
#define LogW logSys.logWarn(__FILE__, __FUNCTION__)
#define LogI logSys.logInfo(__FILE__, __FUNCTION__)

static const std::string log_debug = "log/debug/";
static const std::string log_warn = "log/warn/";   //要改为读入配置文件
static const std::string log_info = "log/info/";

namespace my
{
	class LogSystem
	{
	public:
		LogSystem();
		~LogSystem();

		std::stringstream& logDebug(const char* fileName, const char* funcName);
		std::stringstream& logWarn(const char* fileName, const char* funcName);
		std::stringstream& logInfo(const char* fileName, const char* funcName);

		void preLog(const char* fileName, const char* funcName);
		void endline();
		void outputLogToFile(std::string dir, std::string content);

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

		std::stringstream m_SStream;
		time_t m_Now;
		tm* m_pNow;	
		std::string m_szLogPath;
	};
}



#endif