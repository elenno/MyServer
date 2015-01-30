#pragma once
#include "json/json.h"

#define config_ins boost::detail::thread::singleton<na::config>::instance()

namespace na
{
	class config
	{
	public:
		config(void);
		~config(void);

		void load_config_json(const std::string& dir);
		void load_config_json(Json::Value& config_json_in);
		const Json::Value& get_config_prame(const std::string& key_str);
		const Json::Value& get_config_json();
		int  update_config_file(Json::Value& new_config_json,const std::string& dir);
	private:
		Json::Value config_json;
	};
}


