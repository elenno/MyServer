#ifndef __HELP_FUNCTIONS_H__ 
#define __HELP_FUNCTIONS_H__

#include <boost/thread/detail/singleton.hpp>
#include <string>
#include <json/json.h>

#define Between(X, L, H) (((X) >= (L) && (X) <= (H)))

namespace util
{
	namespace HelpFunctions
	{
		std::string tighten(const std::string& str);
		void threadSleep(int offset);
		void threadSleepSecond(int offset);
		tm* getCurrentTM(tm* pdate);
	};
}



#endif