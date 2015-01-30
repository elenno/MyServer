#include <iostream>
#include <string>
#include "Glog.h"
#include "GLogLevel.h"
#include "GLogColor.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <ctime>
#ifdef _WIN_LOG_
#include <Windows.h>
#endif
#include "json/json.h"
#include "file_system.h"

using namespace std;
using namespace na::util;


namespace na
{
	namespace util
	{
		static GLog* m_pInstance = NULL;
		string GLog::serverName="server";
		string GLog::logPathName="./log/";
		string GLog::lastLogFullFileName="";
		ofstream GLog::logOfstream;
		tm GLog::lastDateTime;
		GLogLevel GLog::fileLogLevel=GLog_TRACE;
		GLogLevel GLog::screenLogLevel=GLog_TRACE;
		class_logLevel_map GLog::classLogLevelMap;
		int GLog::text_color = White;
		GLog& GLog::getInstance()
		{
			if(m_pInstance==NULL)
			{
				m_pInstance = new GLog();
				m_pInstance->setColor(White);
			}

			return *m_pInstance;
		}
	
		void GLog::readClassLogLevelConfig(std::string cfg) //读取单个类日志级别配置
		{
			const std::string file_name = cfg;
			Json::Value logConfig_Value = na::file_system::load_jsonfile_val(file_name);

			fileLogLevel=(GLogLevel)(logConfig_Value["fileLogLevel"].asInt());
			screenLogLevel=(GLogLevel)(logConfig_Value["screenLogLevel"].asInt());
			serverName=logConfig_Value["serverName"].asString();
			logPathName=logConfig_Value["logPathName"].asString();

			classLogLevelMap.clear();

			Json::Value classLevel_Value =logConfig_Value["classLogLevelMap"];
 			for(Json::Value::iterator it=classLevel_Value.begin();it!=classLevel_Value.end();++it)
 			{
				Json::Value& keyValueIt = *it;

				//cout << it.key() << ":" << keyValueIt.asInt() << endl;
				classLogLevelMap[it.key().asString()]=keyValueIt.asInt();
			}
		}
		

		void GLog::readConfig(std::string cfg)
		{
			readClassLogLevelConfig(cfg);
		}

		void GLog::printConfig()
		{
			cout << "Type level : ALL=1,TRACE=2,DEBUG=3,INFO=4,WARN=5,ERROR=6,FATAL=7,OFF=8" << endl;
			cout << "----------------------------------------------------" << endl;
			cout << "Log file level:   " << getGLogLevelName(fileLogLevel) <<endl;
			cout << "Log screen level: " << getGLogLevelName(screenLogLevel) <<endl;
			cout << "Log name:         " << serverName <<endl;
			cout << "Log file dir:	   " << logPathName <<endl;
			cout << "" <<endl;
			cout << "Class log level map size : " << classLogLevelMap.size() << endl;
			class_logLevel_map::iterator it;
			cout << "====================================================" << endl;
			for(it=classLogLevelMap.begin();it!=classLogLevelMap.end();it++)
			{
				//命名空间/类名
				cout << "Function name:" << it->first << " value:" << getGLogLevelName((GLogLevel)(it->second)) << endl;
			}
			cout << "====================================================" << endl;
		}

		string GLog::getGLogLevelName( GLogLevel enumValue )
		{
			switch (enumValue)
			{
			case GLog_ALL:
				return "ALL";
			case GLog_TRACE:
				return "TRACE";
			case GLog_DEBUG:
				return "DEBUG";
			case GLog_INFO:
				return "INFO";
			case GLog_WARN:
				return "WARN";
			case GLog_ERROR:
				return "ERROR";
			case GLog_FATAL:
				return "FATAL";
			case GLog_SYSTEM:
				return "SYS";
			case GLog_OFF:
				return "OFF";
			}
			return "";
		}

		void GLog::openOrCreateFile( string fileName )
		{
			if (logOfstream.is_open())
			{
				cout << "GLog close file:" << lastLogFullFileName << endl;
				logOfstream.close();
			}

			if(boost::filesystem::is_directory(logPathName)==false)
			{
				boost::filesystem::create_directory(logPathName);
			}

			lastLogFullFileName=logPathName+fileName;

			logOfstream.open(lastLogFullFileName.c_str(),ios_base::app);

			cout << "GLog open file:" << lastLogFullFileName << endl;
		}

		bool GLog:: checkAndUpdateLastCreateFileDate()
		{
			boost::posix_time::ptime p = boost::posix_time::from_time_t((unsigned)std::time(0) + 8 * 3600) ;
			struct tm now=boost::posix_time::to_tm(p);
			
			if (now.tm_year != this->lastDateTime.tm_year
				|| now.tm_mon != this->lastDateTime.tm_mon
				|| now.tm_mday != this->lastDateTime.tm_mday
				)
			{
				lastDateTime = now;
				return true;
			}
			else
			{
				lastDateTime = now;
				return false;
			}
		}

		string GLog::getCurrentDateTime()
		{
			return boost::lexical_cast<std::string,int> (lastDateTime.tm_year+1900)+"-"
				+(lastDateTime.tm_mon+1<=9?"0":"")
				+boost::lexical_cast<std::string,int> (lastDateTime.tm_mon+1)+"-"
				+(lastDateTime.tm_mday<=9?"0":"")
				+boost::lexical_cast<std::string,int> (lastDateTime.tm_mday);
		}

		GLog& GLog::preLog(GLogLevel level,GLogColor color,const char* functionName,int lineNumber)
		{
			
			string className=string(functionName);
			isOutPutFile=checkToFileLogLevel(level,className);
			//isOutPutScreen =checkToScreenLogLevel(level,className);
			isOutPutScreen =checkToScreenLogLevel(level,className);
			if(isOutPutFile)
			{
				if(checkAndUpdateLastCreateFileDate())
				{
					openOrCreateFile(this->serverName+"-"+getCurrentDateTime()+".log");
				}

				this->logOfstream << (lastDateTime.tm_hour < 10 ? "0" : "") << lastDateTime.tm_hour << ":" 
					<< (lastDateTime.tm_min < 10 ? "0" : "") << lastDateTime.tm_min << ":" 
					<< (lastDateTime.tm_sec < 10 ? "0" : "") << lastDateTime.tm_sec 
					<< " ["+getGLogLevelName(level)+"]\t";
			}

			if (isOutPutScreen)
			{
				text_color = color;
				//logToScreen("").logToScreen(":").logToScreen(lineNumber).logToScreen("{").logToScreen(functionName).logToScreen("}");
				//std::cout << endl;

				logToScreen(lastDateTime.tm_hour < 10 ? "0" : "").logToScreen(lastDateTime.tm_hour).logToScreen(":");
				logToScreen(lastDateTime.tm_min < 10 ? "0" : "").logToScreen(lastDateTime.tm_min).logToScreen(":");
				logToScreen(lastDateTime.tm_sec < 10 ? "0" : "").logToScreen(lastDateTime.tm_sec);
				setColor(color);
				logToScreen(" ["+getGLogLevelName(level)+"] ");
				setColor(GLogColor(White));
			}

			return getInstance();
		}

		bool GLog::checkToFileLogLevel(GLogLevel level,string& className,bool check_father)
		{
			class_logLevel_map::iterator iter=classLogLevelMap.find(className);
			if(iter!=classLogLevelMap.end())
			{
				if(iter->second<=int(level))
				{
					if(check_father)
					{
						string cs = getClassString(className);
						if(cs=="") return true;
						return checkToFileLogLevel(level,cs,true);
					}
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				if(fileLogLevel<=level)//日志级别小于全局
				{
					string cs = getClassString(className);
					if(cs=="") return true;
					return checkToFileLogLevel(level,cs,true);
				}
				else
				{
					return false;
				}
			}
		}

		bool GLog::checkToScreenLogLevel(GLogLevel level,string& className,bool check_father)
		{
			class_logLevel_map::iterator iter=classLogLevelMap.find(className);
			if(iter!=classLogLevelMap.end())
			{
				if(iter->second<=int(level))
				{
					//if(check_father)
					//{
					//	string cs = getClassString(className);
					//	if(cs=="") return true;
					//	return checkToScreenLogLevel(level,cs,true);
					//}
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				if(screenLogLevel<=level)//日志级别小于全局
				{
					string cs = getClassString(className);
					if(cs=="") return true;
					return checkToScreenLogLevel(level,cs,true);
				}
				else
				{
					return false;
				}
			}
		}

		GLog& GLog::trace(GLogColor color,const char* functionName,int lineNumber )
		{
			preLog(GLog_TRACE,color,functionName,lineNumber);
			return getInstance();
		}

		GLog& GLog::debug(GLogColor color,const char* functionName,int lineNumber )
		{
			preLog(GLog_DEBUG,color,functionName,lineNumber);
			return getInstance();
		}

		GLog& GLog::info(GLogColor color,const char* functionName,int lineNumber)
		{
			preLog(GLog_INFO,color,functionName,lineNumber);
			return getInstance() << " ";
		}

		GLog& GLog::warn(GLogColor color,const char* functionName,int lineNumber)
		{
			preLog(GLog_WARN,color,functionName,lineNumber);
			return getInstance()<< " ";
		}

		GLog& GLog::error(GLogColor color,const char* functionName,int lineNumber)
		{
			preLog(GLog_ERROR,color,functionName,lineNumber);
			return getInstance();
		}

		GLog& GLog::fatal(GLogColor color,const char* functionName,int lineNumber)
		{
			preLog(GLog_FATAL,color,functionName,lineNumber);
			return getInstance();
		}

		GLog& GLog::system(GLogColor color,const char* functionName,int lineNumber)
		{
			preLog(GLog_SYSTEM,color,functionName,lineNumber);
			return getInstance();
		}

		void GLog::setColor(GLogColor color)
		{
			#ifdef _WIN_LOG_
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
				SetConsoleTextAttribute(hConsole,color);
			#else
				printf("\033[%d;40;1m",color); 
			#endif

		}

		GLog &GLog::operator<<(const int &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const short &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const unsigned int &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const unsigned short &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const unsigned long &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const float &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const double &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const char &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const char *msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const std::string &msg)
		{
			logToFile(msg);

			logToScreen(msg);
			if(msg.size()>0)
				if (msg[msg.size()-1] == '\n')
				{
					isOutPutScreen=true;
					isOutPutFile=true;

					setColor(White);
				}

			return getInstance();
		}

		GLog &GLog::operator<<(const time_t &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		GLog &GLog::operator<<(const boost::system::error_code &msg)
		{
			logToFile(msg);
			return logToScreen(msg);
		}

		template <typename F>
		GLog& GLog::logToFile(F f)
		{
			if(isOutPutFile)
			{
				this->logOfstream << f;
				this->logOfstream.flush();
			}

			return getInstance();
		}

		template <typename T>
		GLog& GLog::logToScreen( T t )
		{
			if(isOutPutScreen)
			{
				cout << t ;
			}
			return getInstance();
		}

		std::string GLog::end_line( const char* functionName,int lineNumber )
		{
			return "\n";
			std::string str;
			str.append("\t[").append(functionName).append("(").append( boost::lexical_cast<std::string,int> (lineNumber)).append(")" ).append("]" ).append( "\n");
			//getInstance() << str;
			return str;
		}

		GLog & GLog::operator<<( GLogColor color )
		{
			setColor(color);
			return getInstance();
		}

		std::string GLog::getClassString( std::string function_str )
		{
			size_t f = function_str.find_last_of("::");
			if(f <= function_str.size())
			{
				function_str[f-1]=0;
				std::string class_str = function_str.c_str();
				return class_str;
			}
			return "";
		}

		GLog::~GLog()
		{
			printf("\033[0m");
		}

		void GLog::setScreenLogLvl( GLogLevel lv )
		{
			LogS << "screen log level:\t" << color_green(screenLogLevel) << "--->\t" << color_green(lv) << LogEnd;
			screenLogLevel = lv;
		}

	}
}
