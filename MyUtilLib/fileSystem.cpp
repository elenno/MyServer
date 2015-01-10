#include "fileSystem.h"
#include <json/json.h>
#include <fstream>

std::string util::fileSystem::loadJsonFile(std::string dir)
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

Json::Value util::fileSystem::loadJsonFileEval(std::string dir)
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