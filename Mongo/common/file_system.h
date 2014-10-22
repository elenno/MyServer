#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include <string>
#include "json/json.h"
#include <boost/thread/detail/singleton.hpp>

#define fileSys boost::detail::thread::singleton<FileSystem>::instance()

class FileSystem
{
public:
	FileSystem();
	~FileSystem();

	std::string loadJsonFile(std::string dir);
	Json::Value loadJsonFileEval(std::string dir);

private:
};

#endif