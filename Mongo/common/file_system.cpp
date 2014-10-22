#include "file_system.h"
#include <fstream>

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

std::string FileSystem::loadJsonFile(std::string dir)
{
	std::string content;
	std::string line;
	std::ifstream ifs(dir);
	if (!ifs)
	{
		return content;
	}
	while(getline(ifs, line))
	{
		content.append(line);
	}
	return content;
}

Json::Value FileSystem::loadJsonFileEval(std::string dir)
{
	std::string content = loadJsonFile(dir);
	Json::Reader reader;
	Json::Value json;
	if (!reader.parse(content, json))
	{
		return Json::nullValue;
	}
	return json;
}