#pragma once

#include <string>
#include <map>
#include "GLogLevel.h"
#include "GLogColor.h"
#include <iostream>
#include <fstream>
#include <boost/system/error_code.hpp>

using namespace std;
using namespace na::util;

#define logger na::util::GLog::getInstance()
#define GLogger logger.preLog(__FUNCTION__,__LINE__)

#define GLogTrace(color) \
	logger.trace(color,__FUNCTION__,__LINE__)

#define GLogDebug(color)  \
	logger.debug(color,__FUNCTION__,__LINE__)

#define GLogInfo(color) \
	logger.info(color,__FUNCTION__,__LINE__) 

#define GLogWarn(color)  \
	logger.warn(color,__FUNCTION__,__LINE__)

#define GLogError(color) \
	logger.error(color,__FUNCTION__,__LINE__)

#define GLogFatal(color) \
	logger.fatal(color,__FUNCTION__,__LINE__)

#define GLogSystem(color) \
	logger.system(color,__FUNCTION__,__LINE__)

#define LogT \
	logger.trace(na::util::White,__FUNCTION__,__LINE__)

#define LogD  \
	logger.debug(na::util::Yellow,__FUNCTION__,__LINE__)

#define LogI \
	logger.info(na::util::Green,__FUNCTION__,__LINE__) 

#define LogW  \
	logger.warn(na::util::Pink,__FUNCTION__,__LINE__)

#define LogE \
	logger.error(na::util::Red,__FUNCTION__,__LINE__)

#define LogF \
	logger.fatal(na::util::Red,__FUNCTION__,__LINE__)

#define LogS \
	logger.system(na::util::Green,__FUNCTION__,__LINE__)

#define LogEnd \
	logger.end_line(__FUNCTION__,__LINE__)

#define color_red(A)	Red		<< A <<  logger.getCurrentColor()
#define color_pink(A)	Pink	<< A <<  logger.getCurrentColor()
#define color_green(A)	Green	<< A <<  logger.getCurrentColor()
#define color_white(A)	White	<< A <<  logger.getCurrentColor()
#define color_yellow(A)	Yellow	<< A <<  logger.getCurrentColor()


namespace na
{
	namespace util
	{
		typedef std::map<string,int> class_logLevel_map;
		
		class GLog
		{
		public:
			~GLog();
			static std::string getGLogLevelName(GLogLevel enumValue);
			static std::string getGLogLevelShortName(GLogLevel enumValue);
			static GLogLevel getGLogLevelByValue(int enumValue);

			static GLog& getInstance();//获取日志单例对象引用

			void readConfig(std::string cfg="./instance/log_config.json");//读取配置
			void printConfig();//打印配置
			void setScreenLogLvl(GLogLevel lv); 

			//write log
			GLog& preLog(GLogLevel level,GLogColor color,const char* functionName,int lineNumber);

			GLog& trace(GLogColor color,const char* functionName,int lineNumber);
			GLog& debug(GLogColor color,const char* functionName,int lineNumber);
			GLog& info(GLogColor color,const char* functionName,int lineNumber);
			GLog& warn(GLogColor color,const char* functionName,int lineNumber);
			GLog& error(GLogColor color,const char* functionName,int lineNumber);
			GLog& fatal(GLogColor color,const char* functionName,int lineNumber);
			GLog& system(GLogColor color,const char* functionName,int lineNumber);

			GLogColor getCurrentColor() {return White;}
			std::string end_line(const char* functionName,int lineNumber);
			//设置颜色
			void setColor(GLogColor color);

			GLog &operator<<(const int &msg);
			GLog &operator<<(const short &msg);
			GLog &operator<<(const unsigned int &msg);
			GLog &operator<<(const unsigned short &msg);
			GLog &operator<<(const unsigned long &msg);
			GLog &operator<<(const float &msg);
			GLog &operator<<(const double &msg);
			GLog &operator<<(const char &msg);
			GLog &operator<<(const char *msg);
			GLog &operator<<(const std::string &msg);
			GLog &operator<<(const time_t &msg);
			GLog &operator<<(const boost::system::error_code &msg);
			GLog &operator<<(GLogColor color);

			private:
				static string serverName;
				static string logPathName;
				static string lastLogFullFileName;
				static ofstream logOfstream;
				static tm lastDateTime;
				static GLogLevel fileLogLevel;//全局日志级别
				static GLogLevel screenLogLevel;//屏幕日志打印级别
				static class_logLevel_map classLogLevelMap;//单个类日志级别配置	
				static int text_color;
				bool isOutPutFile;
				bool isOutPutScreen;

				void readClassLogLevelConfig(std::string cfg);

				void openOrCreateFile(string fileName);

				bool checkAndUpdateLastCreateFileDate();
				string getCurrentDateTime();
				std::string getClassString(std::string function_str);

				bool checkToFileLogLevel(GLogLevel level,string& className,bool check_father=false);
				bool checkToScreenLogLevel(GLogLevel level,string& className,bool check_father=false);

				template <typename F>
				GLog& logToFile(F f);

				template <typename T>
				GLog& logToScreen(T t);
		};
	}
}
