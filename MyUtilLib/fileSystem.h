#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include <string>
#include <json/json.h>
#include <boost/thread/detail/singleton.hpp>

namespace my
{
	namespace fileSystem
	{
		std::string loadJsonFile(std::string dir);
		Json::Value loadJsonFileEval(std::string dir);
	}
}

#endif