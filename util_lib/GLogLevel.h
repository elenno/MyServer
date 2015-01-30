#pragma once

#include <string>

namespace na
{
	namespace util
	{
		
//#define getString(X) global##X
//#define NAME(value) #value
		enum GLogLevel
		{
			GLog_ALL=1,//最低等级的，打开所有日志记录
			GLog_TRACE=2,//跟踪
			GLog_DEBUG=3,//调试
			GLog_INFO=4,//信息
			GLog_WARN=5,//警告
			GLog_ERROR=6,//错误
			GLog_FATAL=7,//致命错误
			GLog_SYSTEM,//系统级别
			GLog_OFF//是最高等级的，关闭所有日志记录

		};
	}
}