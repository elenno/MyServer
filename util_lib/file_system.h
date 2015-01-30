#pragma once
#include <string>
#include <vector>
#include <map>
#include "json/json.h"
using namespace std;
namespace na
{
	namespace file_system
	{
		typedef vector<std::string>			json_file_vec;
		typedef vector<Json::Value>		json_value_vec;
		typedef map<int, Json::Value>	json_value_map;
		std::string load_jsonfile(const std::string& file_name);
		Json::Value load_jsonfile_val(const std::string& file_name);
		int load_jsonfiles_from_dir(const std::string& files_path,json_file_vec& vec );
		int load_jsonfiles_from_dir(const std::string& files_path,json_value_vec& vec );
		int load_jsonfiles_from_dir(const std::string& files_path,json_value_map& m, string item_id = "id");

		bool is_flord_exists(const std::string& flordor_file_dir);
		bool is_flord_or_file_exists(const std::string& flordor_file_dir);
		void create_flord(const std::string& flordor_dir);
		int write_file(const std::string& file_path, const std::string& content_str);
		int rename_file(const std::string& flord_path, const std::string& old_file_name, const std::string& new_file_name);
		int copy_file(const std::string& sorce_file_dir, const std::string& target_flord_path, bool is_over_write = false);
		int delete_flord(const std::string& flord_path);
	}
}



