#include "config.h"
#include "file_system.h"
#include <fstream>

namespace na
{
	config::config(void)
	{
		config_json = Json::Value::null;
	}

	config::~config(void)
	{

	}

	void config::load_config_json(const std::string& dir)
	{
		//if (config_json == Json::Value::null)
			config_json = na::file_system::load_jsonfile_val(dir);
	}

	void config::load_config_json(Json::Value& config_json_in)
	{
		config_json = config_json_in;
	}

	const Json::Value& config::get_config_prame(const std::string& key_str)
	{
		if (config_json != Json::Value::null)
			return config_json[key_str];
		return config_json;
	}

	const Json::Value& config::get_config_json()
	{
		return config_json;
	}

	int config::update_config_file(Json::Value& new_config_json,const std::string& dir)
	{
		bool is_change = false;
		for (Json::Value::iterator ite = config_json.begin(); ite != config_json.end(); ++ite)
		{
			std::string key_str = ite.key().asString();
			if(!new_config_json.isMember(key_str))
				continue;
			(*ite) = new_config_json[key_str];
			is_change = true;
		}
		std::ofstream f(dir.c_str());
		std::string config_file = config_json.toStyledString();
		f << config_file;
		f.flush();
		f.close();
		return 0;
	}
}


