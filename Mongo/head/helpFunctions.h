#ifndef __HELP_FUNCTIONS_H__ 
#define __HELP_FUNCTIONS_H__

#include "../common/common.h"
#include <boost/thread/detail/singleton.hpp>

#define helpFunc boost::detail::thread::singleton<HelpFunctions>::instance()

class HelpFunctions
{
public:
	HelpFunctions();
	~HelpFunctions();

	string tighten(const string& str);
};

#endif