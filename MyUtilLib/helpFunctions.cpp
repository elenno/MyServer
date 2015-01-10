#include "helpFunctions.h"
#include <boost/thread/thread_time.hpp>
#include <boost/thread.hpp>
#include <fstream>
#include <boost/date_time/c_time.hpp>

std::string util::HelpFunctions::tighten(const std::string& str)
{
	bool in_string = false;
	std::string tmp;
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++)
	{
		char ch = *iter;
		int a = (unsigned)(ch + 1);
		if (Between(a, 0, 256) && isspace(ch) && in_string == false)
		{
			continue;
		}

		tmp.insert(tmp.end(), ch);
		if (ch == '\"')
		{
			in_string = !in_string;
		}
	}
	return tmp;
}

void util::HelpFunctions::threadSleep(int offset)
{
	boost::system_time time = boost::get_system_time();
	time += boost::posix_time::microsec(offset); 
	boost::thread::sleep(time);
}

void util::HelpFunctions::threadSleepSecond(int offset)
{
	boost::system_time time = boost::get_system_time();
	time += boost::posix_time::seconds(offset); 
	boost::thread::sleep(time);
}

tm* util::HelpFunctions::getCurrentTM(tm* pdate)
{
	time_t now = time(NULL);
	return boost::date_time::c_time::localtime(&now, pdate);
}